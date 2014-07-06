#include "drop_bone_imu.h"

#include <stdio.h>


int main(int argc, char **argv){
    
}

int i2c_write(unsigned char slave_addr, unsigned char reg_addr,
    unsigned char length, unsigned char const *data){
        
}
int i2c_read(unsigned char slave_addr, unsigned char reg_addr,
    unsigned char length, unsigned char *data){
        
}

double open_bus() { 
    if ((fd = open(BBB_I2C_FILE)) < 0) {
        /* ERROR HANDLING: you can check errno to see what went wrong */
        perror("Failed to open the i2c bus");
        return 1;
    }
    if (ioctl(fd, BBB_I2C_FILE, MPU6050_ADDR) < 0) {
        printf("Failed to acquire bus access and/or talk to slave.\n");
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
    
}
inline void __no_operation(){
    
}
