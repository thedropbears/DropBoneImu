#include <stdio.h>
#include <unistd.h>
#include "udp.h"

int main(void) {
    float euler[] = {0, 0, 0};
    while (1) {
        euler[0] += 1.0/180.0*3.14159; //yaw
        //euler[1] += 1.0/180.0*3.14159; //roll
        //euler[2] += 1.0/180.0*3.14159; //pitch
        
        udp_send(euler, 3);
        usleep(100000);
    }
}
