#ifndef drop_bone_imu_h
#define drop_bone_imu_h

#define MPU6050 // The Invensense Motion Driver code needs this

#define BBB_I2C_FILE "/dev/i2c-1"

#define MPU6050_ADDR 0x68
#define MPU6050_WHO_AM_I 0x75

#include "inv_mpu.h"



/* The following functions must be defined for this platform:
 * i2c_write(unsigned char slave_addr, unsigned char reg_addr,
 *      unsigned char length, unsigned char const *data)
 * i2c_read(unsigned char slave_addr, unsigned char reg_addr,
 *      unsigned char length, unsigned char *data)
 * delay_ms(unsigned long num_ms)
 * get_ms(unsigned long *count)
 * reg_int_cb(void (*cb)(void), unsigned char port, unsigned char pin)
 * labs(long x)
 * fabsf(float x)
 * min(int a, int b)
 */

#define log_i printf

int main(int argc, char **argv);

static int fd; // file descriptor for the I2C bus

int i2c_write(unsigned char slave_addr, unsigned char reg_addr,
    unsigned char length, unsigned char const *data);
int i2c_read(unsigned char slave_addr, unsigned char reg_addr,
    unsigned char length, unsigned char *data);

int open_bus();

void delay_ms(unsigned long num_ms);
void get_ms(unsigned long *count);
void reg_int_cb(struct int_param_s *);

inline int min ( int a, int b );
inline void __no_operation();

#endif
