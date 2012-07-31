#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "include/intheory.h"
#include "include/network.h"
#include "include/util.h"

struct sockaddr_in get_remote_server(int node) {
  struct sockaddr_in serveraddr;
  struct hostent *server;
  char *hostname;
  int port;

  hostname = get_address(node);
  port = get_port(node);;

  server = gethostbyname(hostname);
  discard(hostname);
  
  if (server == NULL) {
    error("ERROR, no such host as %s", hostname);
    assert(0);
  }

  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;

  bcopy((char *)server->h_addr, (char *)&serveraddr.sin_addr.s_addr, server->h_length);
  serveraddr.sin_port = htons(port);
  return serveraddr;
}

int send_intheory(int node, message *msg) {
  int sockfd, size;
  struct sockaddr *serveraddr;
  struct sockaddr_in sin;
  struct sockaddr_un sun;
  

  if (intheory_remote) {
    sockfd = it_remote_socket();
    sin = get_remote_server(node);
    size = sizeof(sin);
    serveraddr = (struct sockaddr *) &sin;
  } else {
    sockfd = it_domain_socket();
    sun = it_domain_sockaddr(node, FALSE);  
    size = sizeof(sun);
    serveraddr = (struct sockaddr *) &sun;
  }
  

  trace("connecting");
  if (connect(sockfd, serveraddr, size) < 0) {
    error("ERROR connecting");
    discard(msg);
    return 0;
  }
  trace("writing...");
  int n = write(sockfd, msg, sizeof(message));
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


