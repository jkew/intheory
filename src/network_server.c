#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "include/intheory.h"
#include "include/network.h"

int server_continue = 1;
pthread_t recv_thread;


int open_socket(int port) {
  int socketfd;
  struct sockaddr_in servaddr;
  if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
    { assert(0); return -1; }
 
  memset(&servaddr, 0, sizeof(struct sockaddr_in));
  servaddr.sin_family      = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port        = htons(port);
  memset(&(servaddr.sin_zero), '\0', 8);

  if(bind(socketfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    perror("ERROR");
    error("Bind error\n");
    assert(0);
  }
  if (listen(socketfd, 10) < 0){
    error("Listen ERROR\n");
    assert(0);
  }

  struct timeval timeout;      
  timeout.tv_sec = 1;
  timeout.tv_usec = 0;
  if (setsockopt (socketfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
		  sizeof(timeout)) < 0) {
    error("setsockopt failed\n");
    assert(0);
  }
  
  if (setsockopt (socketfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,
		  sizeof(timeout)) < 0) {
    error("setsockopt failed\n");
    assert(0);
  }

  if (setsockopt (socketfd, SOL_SOCKET, SO_REUSEADDR, (char *)&timeout,
		  sizeof(timeout)) < 0) {
    error("setsockopt failed\n");
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

  if ((sockfd = open_socket(get_port(my_id()))) < 0) { assert(0); return; }
  FD_SET(sockfd, &master);
  fdmax = sockfd;
  info("Server started");
  while(server_continue) {
    read_fds = master;
    if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) { asser(0); return; }
    // run through all fds
    for(i = 0; i <= fdmax; i++) {
      if(FD_ISSET(i, &read_fds)) {
	if (i == sockfd) {
	  // new connections
	  int len = sizeof(clientaddr);
	  int newfd = accept(sockfd, (struct sockaddr *)&clientaddr, &len);
	  if (newfd >= 0) {
	    FD_SET(newfd, &master); 
	    if(newfd > fdmax) { fdmax = newfd; }
	    trace("Connection from %s on socket %d", inet_ntoa(clientaddr.sin_addr), newfd);	  
	  } else {
	    printf("conitinue;.... \n");
	  }
	} else {
          message msg;
	  int bytes_read;
	  // existing connection
	  bytes_read = recv(i, &msg, sizeof(message), 0);
	  
	  if (bytes_read == sizeof(message)) {
	    if (!crc_valid(&msg)) {
	      error("CRC not valid on message");
	      break;
	    } else {
	      trace("Valid message received");
	    }
	    if (msg.type == EXIT) {
	      info("Received exit message");
	      server_continue = 0;
	      break;
	    }
	    add_message(create_message(msg.from, msg.to, msg.ticket, msg.type, msg.slot, msg.value));
	  }

          if (bytes_read > 0 && bytes_read != sizeof(message)) {
	    error("Invalid read");
	  }
	  if (bytes_read <= 0) {
	    if (bytes_read == 0) {
	      trace("socket hung up");
	    } else {
	      error("socket read error");
	      assert(0);
	    }
	    close(i);
	    FD_CLR(i, &master);
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
  //server_continue = 0;
  send_intheory(my_id(), create_message(my_id(), my_id(), -1, EXIT, -1, -1));
  usleep(100000);
  info("Waiting for network server to stop");
  pthread_join(&recv_thread, 0);
  info("Server stopped");
}


