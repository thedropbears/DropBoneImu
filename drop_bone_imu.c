#include "drop_bone_imu.h"
#include "udp.h"

#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <time.h>

static float last_euler[3] = { 99.9, 99.9, 99.9 };
static float quat_offset[4] = { 0.0F };
static int fd; // file descriptor for the I2C bus
static signed char gyro_orientation[9] = {0,  1,  0,
        -1, 0,  0,
        0,  0,  1};
int no_interrupt_flag;
int verbose_flag;
int print_usage_flag;
int silent_flag;
int no_broadcast_flag; //useful for testing when offline

int main(int argc, char** argv){
    parse_args(argc, argv);
    if(print_usage_flag) {
        print_usage();
        return 0;
    }
    init();
    short accel[3], gyro[3], sensors[1];
    long quat[4];
    unsigned long timestamp;
    unsigned char more[0];
    struct pollfd fdset[1];
    char buf[1];

    // File descriptor for the GPIO interrupt pin
    int gpio_fd = open(GPIO_INT_FILE, O_RDONLY | O_NONBLOCK);

    // Create an event on the GPIO value file
    memset((void*)fdset, 0, sizeof(fdset));
    fdset[0].fd = gpio_fd;
    fdset[0].events = POLLPRI;
    time_t sec, current_time; // set to the time before calibration

    time(&sec);
    if(!silent_flag) {
        printf("Read system time\n");
        printf("Calibrating\n");
    }

    while (1){
        // Blocking poll to wait for an edge on the interrupt
        if(!no_interrupt_flag) {
            poll(fdset, 1, -1);
        }
            
        if (no_interrupt_flag || fdset[0].revents & POLLPRI) {
            // Read the file to make it reset the interrupt
            if(!no_interrupt_flag) {
                read(fdset[0].fd, buf, 1);
			}

            int fifo_read = dmp_read_fifo(gyro, accel, quat, &timestamp, sensors, more);
            if (fifo_read != 0) {
                //printf("Error reading fifo.\n");
                continue;
            }

            float angles[NOSENTVALS];
            rescale_l(quat, angles+9, QUAT_SCALE, 4);

            if (!quat_offset[0]) {
                advance_spinner(); // Heartbeat to let the user know we are running"
                euler(angles+9, angles); // Determine calibration based on settled Euler angles
                // check if the IMU has finished calibrating
                time(&current_time);
                // check if more than CALIBRATION_TIME seconds has passed since calibration started
                if((fabs(last_euler[0]-angles[0]) < THRESHOLD
                        && fabs(last_euler[1]-angles[1]) < THRESHOLD
                        && fabs(last_euler[2]-angles[2]) < THRESHOLD)
                        || difftime(current_time, sec) > CALIBRATION_TIME) {
                    if(!silent_flag) {
                        printf("\nCALIBRATED! Threshold: %f Elapsed time: %f\n", CALIBRATION_TIME, difftime(current_time, sec));
                        printf("CALIBRATED! Threshold: %.5f Errors: %.5f %.5f %.5f\n", THRESHOLD, fabs(last_euler[0]-angles[0]), last_euler[1]-angles[1], last_euler[2]-angles[2]);
				    }
                    // the IMU has finished calibrating
                    int i;
                    quat_offset[0] = angles[9]; // treat the w value separately as it does not need to be reversed
                    for(i=1;i<4;++i){
                        quat_offset[i] = -angles[i+9];
                    }
                }
                else {
                    memcpy(last_euler, angles, 3*sizeof(float));
                }
            }
            else {
                q_multiply(quat_offset, angles+9, angles+9); // multiply the current quaternstion by the offset caputured above to re-zero the values
                // rescale the gyro and accel values received from the IMU from longs that the
                // it uses for efficiency to the floats that they actually are and store these values in the angles array
                rescale_s(gyro, angles+3, GYRO_SCALE, 3);
                rescale_s(accel, angles+6, ACCEL_SCALE, 3);
                // turn the quaternation (that is already in angles) into euler angles and store it in the angles array
                euler(angles+9, angles);
                if(!silent_flag && verbose_flag) {
                    printf("Yaw: %+5.1f\tPitch: %+5.1f\tRoll: %+5.1f\n", angles[0]*180.0/PI, angles[1]*180.0/PI, angles[2]*180.0/PI);
                }
                // send the values in angles over UDP as a string (in udp.c/h)
                if(!no_broadcast_flag) {
                    udp_send(angles, 13);
                }
            }
        }
    }
}

void parse_args(int argc, char** argv) {
    int ch;
    no_interrupt_flag = 0;
    verbose_flag = 0;
    silent_flag = 0;
    print_usage_flag = 0;
    no_broadcast_flag = 0;
    //flag i for no interrupt, v for no verbose
    while((ch=getopt(argc, argv, "ivsbh?")) != -1) {
        switch(ch) {
            case 'i': no_interrupt_flag=1; break;
            case 'v': verbose_flag=1; break;
            case 's': silent_flag=1; break;
            case 'b': no_broadcast_flag=1; break;
            case 'h':
            case '?': print_usage_flag=1; break;
        }
    }
}

void print_usage() {
    printf("DropBoneImu- software interface and broadcast client for MPU 6050:\n");
    printf("Usage: dropboneimu [-i] [-v] [-s] [-b] [-h, -?]\n\n");
    printf("Arguments:\n");
    printf("-i\tDisable interrupt pin. Will make code run if not wired up with interrupt pin\n");
    printf("-v\tVerbose mode. Print out yaw, pitch and roll as they are received\n");
    printf("-s\tSilent mode. Do not print anything\n");
    printf("-b\tNo broadcasts. Stops the udp server from broadcasting information received from the MPU over UDP\n");
    printf("-h, -?\tDisplay this usage message, then exit\n");
}

int init(){
    open_bus();
    unsigned char whoami=0;
    i2c_read(MPU6050_ADDR, MPU6050_WHO_AM_I, 1, &whoami);
    if(!silent_flag) {
        printf("WHO_AM_I: %x\n", whoami);
    }
    struct int_param_s int_param;
    if(!silent_flag) {
        printf("MPU init: %i\n", mpu_init(&int_param));
        printf("MPU sensor init: %i\n", mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL));
        printf("MPU configure fifo: %i\n", mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL));
        printf("DMP firmware: %i\n ",dmp_load_motion_driver_firmware());
        printf("DMP orientation: %i\n ",dmp_set_orientation(
            inv_orientation_matrix_to_scalar(gyro_orientation)));
    }
    unsigned short dmp_features = DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_TAP | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO | DMP_FEATURE_GYRO_CAL;
    if(!silent_flag) {
        printf("DMP feature enable: %i\n", dmp_enable_feature(dmp_features));
        printf("DMP set fifo rate: %i\n", dmp_set_fifo_rate(DEFAULT_MPU_HZ));
        printf("DMP enable %i\n", mpu_set_dmp_state(1));
    }
    if(!no_interrupt_flag) {
        mpu_set_int_level(1); // Interrupt is low when firing
        dmp_set_interrupt_mode(DMP_INT_CONTINUOUS); // Fire interrupt on new FIFO value
	}
    return 0;
}

int i2c_write(unsigned char slave_addr, unsigned char reg_addr,
        unsigned char length, unsigned char const *data){
    unsigned char tmp[length+1];
    tmp[0] = reg_addr;
    memcpy(tmp+1, data, length);
    if (write(fd, tmp, length+1) != length + 1){
        return -1;
    }
    return 0; 
}
int i2c_read(unsigned char slave_addr, unsigned char reg_addr,
        unsigned char length, unsigned char *data){
    if (write(fd,&reg_addr, 1) != 1){
        return -1;
    }
    if  (read(fd,data, length) != length){
        return -2;
    }

    return 0;
}

int open_bus() { 
    if ((fd = open(BBB_I2C_FILE, O_RDWR)) < 0) {
        /* ERROR HANDLING: you can check errno to see what went wrong */
        perror("Failed to open the i2c bus");
        return 1;
    }
    if (ioctl(fd, I2C_SLAVE, MPU6050_ADDR) < 0) {
        perror("Failed to acquire bus access and/or talk to slave.\n");
        /* ERROR HANDLING; you can check errno to see what went wrong */
        return 1;
    }
    return 0;
}

// multiply two quaternions
int q_multiply(float* q1, float* q2, float* result) {
    float tmp[4];
    tmp[0] = q1[0]*q2[0] - q1[1]*q2[1] - q1[2]*q2[2] - q1[3]*q2[3];
    tmp[1] = q1[0]*q2[1] + q1[1]*q2[0] + q1[2]*q2[3] - q1[3]*q2[2];
    tmp[2] = q1[0]*q2[2] - q1[1]*q2[3] + q1[2]*q2[0] + q1[3]*q2[1];
    tmp[3] = q1[0]*q2[3] + q1[1]*q2[2] - q1[2]*q2[1] + q1[3]*q2[0];
    memcpy(result, tmp, 4*sizeof(float));
    return 0;
}

// rescale an array of longs by scale factor into an array of floats
int rescale_l(long* input, float* output, float scale_factor, char length) {
    int i;
    for(i=0;i<length;++i)
        output[i] = input[i] * scale_factor;
    return 0;
}

// rescale an array of shorts by scale factor into an array of floats
int rescale_s(short* input, float* output, float scale_factor, char length) {
    int i;
    for(i=0;i<length;++i)
        output[i] = input[i] * scale_factor;
    return 0;
}

void delay_ms(unsigned long num_ms){

}
void get_ms(unsigned long *count){

}
void reg_int_cb(struct int_param_s *param){

}

inline int min ( int a, int b ){
    return a < b ? a : b;
}
inline void __no_operation(){

}

void euler(float* q, float* euler_angles) {
    euler_angles[0] = -atan2(2*q[1]*q[2] - 2*q[0]*q[3], 2*q[0]*q[0] + 2*q[1]*q[1] - 1); // psi, yaw
    euler_angles[1] = asin(2*q[1]*q[3] + 2*q[0]*q[2]); // phi, pitch
    euler_angles[2] = -atan2(2*q[2]*q[3] - 2*q[0]*q[1], 2*q[0]*q[0] + 2*q[3]*q[3] - 1); // theta, roll
}

// Functions for setting gyro/accel orientation
unsigned short inv_row_2_scale(const signed char *row)
{
    unsigned short b;

    if (row[0] > 0)
        b = 0;
    else if (row[0] < 0)
        b = 4;
    else if (row[1] > 0)
        b = 1;
    else if (row[1] < 0)
        b = 5;
    else if (row[2] > 0)
        b = 2;
    else if (row[2] < 0)
        b = 6;
    else
        b = 7;      // error
    return b;
}

unsigned short inv_orientation_matrix_to_scalar(
        const signed char *mtx)
{
    unsigned short scalar;

    /*
       XYZ  010_001_000 Identity Matrix
       XZY  001_010_000
       YXZ  010_000_001
       YZX  000_010_001
       ZXY  001_000_010
       ZYX  000_001_010
     */

    scalar = inv_row_2_scale(mtx);
    scalar |= inv_row_2_scale(mtx + 3) << 3;
    scalar |= inv_row_2_scale(mtx + 6) << 6;


    return scalar;
}

void advance_spinner() {
    static char bars[] = { '/', '-', '\\', '|' };
    static int nbars = sizeof(bars) / sizeof(char);
    static int pos = 0;
    if(!silent_flag) {
        printf("%c\b", bars[pos]);
    }
    fflush(stdout);
    pos = (pos + 1) % nbars;
}
