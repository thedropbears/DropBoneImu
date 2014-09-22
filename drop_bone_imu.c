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
