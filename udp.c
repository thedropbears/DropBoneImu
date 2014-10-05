#include "udp.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

static int sockfd; // file descriptor for the socket
static struct sockaddr_in their_addr; // connector's address information

void set_up_socket()
{
	int broadcast = 1;
	
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		printf("Error: creating socket\n");
		exit(1);
	}
	
	if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast) == -1) {
		printf("Error: setsockopt\n");
		exit(1);
	}
	
	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(SERVPORT);
	memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);
}

int udp_send(float *data, unsigned int length)
{
	int i, bytes_sent, stringpos;
	char *msg;
	char temp[FLEN];
	msg = malloc(FLEN*length); // allocate FLEN characters for each float in data
	if (sockfd == 0)
		set_up_socket();
	
	// convert the array of floats into a string in msg
	// we have already turned the first element of data into a string, so we set i to 1
	for(i = 0, stringpos = 0; i<length; ++i, stringpos+=strlen(temp)) {
		// we copy the ith element of data into a buffer in which
		// there are 12 characters allocated for each element of data)
		sprintf(temp, "%f,", data[i]);
		strcpy(msg+stringpos,temp);
	}
	
	
	bytes_sent = sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *)&their_addr, sizeof their_addr);
	printf("%s\n", msg);
	free(msg);
	return bytes_sent;
}
