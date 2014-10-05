#include "udp.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

static int sockfd = 0; // file descriptor for the socket
static struct sockaddr_in their_addr; // connector's address information

int set_up_socket()
{
	int broadcast = 1;
	
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		printf("Error: creating socket\n");
		return -1;
	}
	
	if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast) == -1) {
		printf("Error: setsockopt\n");
		return -1;
	}
	
	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(SERVPORT);
	memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);
	return 0;
}

int udp_send(float *data, unsigned int length)
{
	int i, bytes_sent;
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
	
	
	bytes_sent = sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *)&their_addr, sizeof their_addr);
	//printf("%s\n", msg);
	free(msg);
	return bytes_sent;
}
