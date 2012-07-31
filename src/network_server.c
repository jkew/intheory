#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include<sys/un.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "include/intheory.h"
#include "include/network.h"
#include "include/util.h"

int server_continue = 1;
pthread_t recv_thread;
int intheory_remote = 1;


void set_it_socket_opts(int socketfd) {
  struct timeval timeout;
  memset(&timeout, 0, sizeof(struct timeval));
  timeout.tv_sec = 1;
  timeout.tv_usec = 0;
  int optval = 1;
  if (setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, &timeout,
		  sizeof(timeout)) < 0) {
    error("setsockopt failed\n");
    assert(0);
  }
  
  if (setsockopt(socketfd, SOL_SOCKET, SO_SNDTIMEO, &timeout,
		  sizeof(timeout)) < 0) {
    error("setsockopt failed\n");
    assert(0);
  }
        
  if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &optval,
		  sizeof(optval)) < 0) {
    error("setsockopt failed\n");
    assert(0);
  }
}

int it_remote_socket() {
  int sockfd;

  trace("opening socket");
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)  {
    error("ERROR opening socket");
    assert(0);
  }
  set_it_socket_opts(sockfd);
  return sockfd;
}

int it_domain_socket() {
  int socketfd;
  if ((socketfd = socket(AF_LOCAL, SOCK_STREAM, 0)) == -1) 
    { assert(0); return -1; }
  return socketfd;
}

struct sockaddr_un it_domain_sockaddr(int id, int delete) {
  char file[256];
  snprintf(file, 256, "/tmp/intheory.server.%d", id);
  struct sockaddr_un servaddr;
  size_t size;
  memset(&servaddr, 0, sizeof(struct sockaddr_un));
  servaddr.sun_family      = AF_LOCAL;
  strcpy(servaddr.sun_path, file);
  if (delete) unlink(file);
  return servaddr;
}


int open_port(int port) {
  int socketfd = it_remote_socket();
  struct sockaddr_in servaddr;
 
  memset(&servaddr, 0, sizeof(struct sockaddr_in));
  servaddr.sin_family      = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port        = htons(port);
  memset(&(servaddr.sin_zero), '\0', 8);

  if(bind(socketfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    error("Bind error\n");
    assert(0);
  }

  if (listen(socketfd, 10) < 0){
    error("Listen ERROR\n");
    assert(0);
  }

  return socketfd;
}

int open_domain(int id) {
  int socketfd = it_domain_socket();
  struct sockaddr_un servaddr = it_domain_sockaddr(id, TRUE);

  if(bind(socketfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    error("Bind error\n");
    assert(0);
  }  
     
  if (listen(socketfd, 10) < 0){
    error("Listen ERROR\n");
    assert(0);
  }

  return socketfd;
}

void server(void *args) {
  fd_set master;
  fd_set read_fds;
  struct sockaddr_in clientaddr;
  int sockfd;
  int fdmax;
  int i;

  FD_ZERO(&master);
  FD_ZERO(&read_fds);

  if (intheory_remote) {
    if ((sockfd = open_port(get_port(my_id()))) < 0) { assert(0); return; }
  } else {
    if ((sockfd = open_domain(my_id())) < 0) { assert(0); return; }
  }
  FD_SET(sockfd, &master);
  fdmax = sockfd;
  info("Server started");
  while(server_continue) {
    read_fds = master;
    if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) { assert(0); return; }
    // run through all fds
    for(i = 0; i <= fdmax; i++) {
      if(FD_ISSET(i, &read_fds)) {
	if (i == sockfd) {
	  // new connections
	  int len = sizeof(clientaddr);
	  trace("Waiting on accept");
	  int newfd = accept(sockfd, (struct sockaddr *)&clientaddr, &len);
	  if (newfd >= 0) {
	    FD_SET(newfd, &master); 
	    if(newfd > fdmax) { fdmax = newfd; }
	    info("Connection from %s on socket %d", inet_ntoa(clientaddr.sin_addr), newfd);	  
	  } else {
	  }
	} else {
          message msg;
	  int bytes_read;
	  // existing connection
	  bytes_read = recv(i, &msg, sizeof(message), 0);
	  
	  if (bytes_read == sizeof(message)) {
	    if (!crc_valid(&msg)) {
	      error("CRC not valid on message");
	      close(i);
	      FD_CLR(i, &master);
	      break;
	    } else {
	      info("Valid message received");
	    }
	    if (msg.type == EXIT) {
	      info("Received exit message");
	      server_continue = 0;
	      break;
	    }
	    if (server_continue)
	      add_message(create_message(msg.from, msg.to, msg.ticket, msg.type, msg.slot, msg.value, msg.flags));
	    close(i);
	    FD_CLR(i, &master);
	  }

          if (bytes_read > 0 && bytes_read != sizeof(message)) {
	    error("Invalid read");

	  }
	  if (bytes_read <= 0) {
	    if (bytes_read == 0) {
	      info("socket hung up");
	      close(i);
	      FD_CLR(i, &master);
	    } else {
	      error("socket read error");
	      close(i);
	      FD_CLR(i, &master);
	    }
	  }
	}
      }
    }
  }
  // close all sockets
  for(i = 0; i <= fdmax; i++) {
    if(FD_ISSET(i, &read_fds)) {
      close(i);
      FD_CLR(i, &master);
    }
  }
  close(sockfd);
  info("Server Terminating...");
 }

void start_server() {
  assert(my_id() >= 0);
  server_continue = 1;
  pthread_create(&recv_thread, 0, server, 0);
}

void stop_server() {
  server_continue = 0;
  send_intheory(my_id(), create_message(my_id(), my_id(), -1, EXIT, -1, -1, 0));
  info("Waiting for network server to stop");
  pthread_join(&recv_thread, 0);
  info("Server stopped");
}


