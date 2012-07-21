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
  
  trace("opening socket");
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)  {
    error("ERROR opening socket");
    return 0;
  }

  struct timeval timeout;
  memset(&timeout, 0, sizeof(struct timeval));
  timeout.tv_sec = 1;
  timeout.tv_usec = 0;

  if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout,
		  sizeof(timeout)) < 0) {
    error("setsockopt failed\n");
    assert(0);
  }
  
  if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout,
		  sizeof(timeout)) < 0) {
    error("setsockopt failed\n");
    assert(0);
  }

  server = gethostbyname(hostname);
  discard(hostname);
  
  if (server == NULL) {
    error("ERROR, no such host as %s", hostname);
    assert(0);
    discard(msg);
    return 0;
  }

  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;

  bcopy((char *)server->h_addr, 
  (char *)&serveraddr.sin_addr.s_addr, 
  	server->h_length);
  serveraddr.sin_port = htons(port);
  trace("connecting");
  if (connect(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) {
    error("ERROR connecting");
    discard(msg);
    return 0;
  }
  trace("writing...");
  n = write(sockfd, msg, sizeof(message));
  if (n < 0) { 
    error("ERROR writing to socket");
    close(sockfd);
    discard(msg);
    return 0;
  }
  log_message("send_intheory", msg);
  close(sockfd);
  discard(msg);
  return 1;
}


