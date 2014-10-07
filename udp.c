#include "udp.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <ifaddrs.h>
#include <string.h>

static int sockfd = -1; // file descriptor for the socket
static struct ifaddrs *ifap; // linked list of broadcast interfaces

int set_up_socket()
{
	struct ifaddrs *ifa;
	int broadcast = 1;
	
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		printf("Error: creating socket\n");
		return -1;
	}
	
	if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast) == -1) {
		printf("Error: setsockopt\n");
		return -1;
	}
	
	if((getifaddrs(&ifap)) == -1) {
		printf("Error: obtaining network interface information (getifaddrs)");
		return -1;
	}
	
	// set ports for the broadcast addresses
	for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr->sa_family==AF_INET) {
            struct sockaddr_in * ba = (struct sockaddr_in *)ifa->ifa_broadaddr;
            ba->sin_port = htons(SERVPORT);
        }
    }

	return 0;
}

int udp_send(float *data, unsigned int length)
{
	int i, bytes_sent, return_flag;
	struct ifaddrs *ifa;
	char *msg;
	msg = malloc(FLEN*length); // allocate FLEN characters for each float in data
	if (sockfd == -1)
		set_up_socket();
	
	sprintf(msg, "%f", data[0]);
	// convert the array of floats into a string in msg
	// we have already turned the first element of data into a string, so we set i to 1
	for(i = 1; i<length; ++i) {
		// we copy the ith element of data into a buffer in which
		// there are 12 characters allocated for each element of data)
		sprintf(msg, "%s,%f", msg, data[i]);
	}
	
	for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
		return_flag = 0;
		if (ifa->ifa_addr->sa_family==AF_INET) {
			struct sockaddr_in *ba = (struct sockaddr_in *)ifa->ifa_broadaddr;
			bytes_sent = sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *) ba, sizeof(struct sockaddr));
			if (bytes_sent == -1) {
				printf("Error: sendto function failed on interface: %s\n", ifa->ifa_name);
				return_flag = -1;
			}
		}
	}
	//printf("%s\n", msg);
	free(msg);
	return return_flag;
}
