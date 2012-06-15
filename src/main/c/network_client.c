#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "include/intheory.h"
#include "include/network.h"
#define INITIAL_RING_SIZE 64

int send_intheory(int node, message *msg) {
  int sockfd, port, n;
  struct sockaddr_in serveraddr;
  struct hostent *server;
  char *hostname;

  hostname = get_address(node);
  port = get_port(node);;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) 
    error("ERROR opening socket");

  server = gethostbyname(hostname);
  discard(hostname);
  
  if (server == NULL) {
    fprintf(stderr,"ERROR, no such host as %s\n", hostname);
    exit(0);
  }

  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  //char * one = (char *)server->h_addr;
  //char * two = (char *)&serveraddr.sin_addr.s_addr;
  //serveraddr.sin_addr.s_addr = server->h_addr;
  bcopy((char *)server->h_addr, 
  (char *)&serveraddr.sin_addr.s_addr, 
  	server->h_length);
  serveraddr.sin_port = htons(port);

  if (connect(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) 
    error("ERROR connecting");

  n = write(sockfd, msg, sizeof(message));
  if (n < 0) 
    error("ERROR writing to socket");

  close(sockfd);
}


