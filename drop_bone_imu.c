#include "drop_bone_imu.h"
#include "udp.h"

#include <stdio.h>
#include <linux/i2c-dev.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>
#include <sys/ioctl.h>

static int fd; // file descriptor for the I2C bus
static signed char gyro_orientation[9] = {1,  0,  0,
                                           0, 1,  0,
                                           0,  0,  1};

int main(int argc, char **argv){

	init();
	short accel[3], gyro[3], sensors[1];
	long quat[4];
	unsigned long timestamp;
	unsigned char more[0];
        
        while (1){
        short status;
        mpu_get_int_status(&status);
        if (! (status & MPU_INT_STATUS_DMP) ){
            continue;
        }
    
        int fifo_read = dmp_read_fifo(gyro, accel, quat, &timestamp, sensors, more);

        if (fifo_read != 0) {
            printf("Error reading fifo.\n");
        }

        if (fifo_read == 0 && sensors[0]) {
            float angles[3];
euler(quat, angles);
printf("Yaw: %+5.1f\tRoll: %+5.1f\tPitch: %+5.1f\n", angles[0]*180.0/PI, angles[1]*180.0/PI, angles[2]*180.0/PI);
        udp_send(angles, 3);
        }
    }

}

int init(){
	open_bus();
    unsigned char whoami=0;
    i2c_read(MPU6050_ADDR, MPU6050_WHO_AM_I, 1, &whoami);
    printf("WHO_AM_I: %x\n", whoami);
	struct int_param_s int_param;
	printf("MPU init: %i\n", mpu_init(&int_param));
	printf("MPU sensor init: %i\n", mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL));
	printf("MPU configure fifo: %i\n", mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL));
	printf("DMP firmware: %i\n ",dmp_load_motion_driver_firmware());
	printf("DMP orientation: %i\n ",dmp_set_orientation(
            inv_orientation_matrix_to_scalar(gyro_orientation)));
    unsigned short dmp_features = DMP_FEATURE_LP_QUAT | DMP_FEATURE_TAP | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO | DMP_FEATURE_GYRO_CAL;
	printf("DMP feature enable: %i\n", dmp_enable_feature(dmp_features));
	printf("DMP set fifo rate: %i\n", dmp_set_fifo_rate(DEFAULT_MPU_HZ));
	printf("DMP enable %i\n", mpu_set_dmp_state(1));
	mpu_set_int_level(1); // Interrupt is low when firing
    dmp_set_interrupt_mode(DMP_INT_CONTINUOUS); // Fire interrupt on new FIFO value
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

void euler(long* quat, float* euler_angles) {
float q[4];
unsigned char i=0;
for (i=0; i<4; ++i) {
q[i] = (float)quat[i] / QUAT_SCALE;
}
euler_angles[0] = -atan2(2*q[1]*q[2] - 2*q[0]*q[3], 2*q[0]*q[0] + 2*q[1]*q[1] - 1); // psi, yaw
euler_angles[1] = asin(2*q[1]*q[3] + 2*q[0]*q[2]); // theta, roll
euler_angles[2] = atan2(2*q[2]*q[3] - 2*q[0]*q[1], 2*q[0]*q[0] + 2*q[3]*q[3] - 1); // phi, pitch
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
