#include <"udp.h">

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

static struct sockaddr_in their_addr; // connector address
static int sockfd; // file descriptor for the socket
static int so_broadcast = TRUE;
static struct sockaddr_in their_addr // connector's address information

// TODO: write broadcast function

void set_up_socket()
{
	struct addrinfo hints;
	struct addrinfo *servinfo;
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	
	getaddrinfo(NULL, SERVPORT, &hints, &servinfo);
	
	if ((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1) {
		printf("Error: creating socket\n");
		exit(1);
	}
	
	if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, so_broadcast, sizeof(so_broadcast)) == -1) {
		printf("Error: setsockopt\n");
		exit(1);
	}
	
	short port = (short) SERVPORT;
	
	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(port);
	inet_aton(SERVPORT, &their_addr.sin_addr);
	memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);
}

int udp_send(float *data, unsigned int length)
{
	int i, j, position, bytes_sent;
	char *msg[100], *temp;
	if (sockfd == 0)
		set_up_socket();
	
	// convert the array of floats into a string in msg
	for(i = 0, position = 0; i<length; ++i, position += j) {
		snprintf(temp,10,"%f",*data[i]);
		// read temp into the end of msg
		for(j = 0; temp[j] != '\0'; ++j)
			msg[position + j] = temp[j];
	}
	
	bytes_sent = sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *)&their_addr, sizeof their_addr);
	return bytes_sent;
}
