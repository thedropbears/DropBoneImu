#include "drop_bone_imu.h"

#include <stdio.h>
#include <linux/i2c-dev.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>


int main(int argc, char **argv){

	init();
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
	unsigned short dmp_features = DMP_FEATURE_LP_QUAT | DMP_FEATURE_TAP | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO | DMP_FEATURE_GYRO_CAL;
	printf("DMP feature enable: %i\n", dmp_enable_feature(dmp_features));
	printf("DMP set fifo rate: %i\n", dmp_set_fifo_rate(DEFAULT_MPU_HZ));
	printf("DMP enable %i\n", mpu_set_dmp_state(1));
	mpu_set_int_level(1); // Interrupt is low when firing
    dmp_set_interrupt_mode(DMP_INT_CONTINUOUS); // Fire interrupt on new FIFO value
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
