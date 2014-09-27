#ifndef udp_h
#define udp_h

#define BROADCASTADDR "255.255.255.255"
#define SERVPORT "8889" // the port on which the packets are broadcast on

static int sockfd; // static file descriptor for the socket
static int so_broadcast = TRUE;
static struct sockaddr_in their_addr // connector's address information

void set_up_socket(); // set up the socket, set up broadcast and bind a port to the socket

void broadcast(int yaw, int pitch, int roll); //TODO

#endif
