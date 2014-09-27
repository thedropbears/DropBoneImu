#include <"udp.h">

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

static struct sockaddr_in their_addr; // connector address

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
	
	if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1){
		printf("Error: bind\n");
		exit(1);
	}
	
	short port = (short) SERVPORT;
	
	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(port);
	inet_aton(SERVPORT, &their_addr.sin_addr);
	memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);
}
