#include "include/network.h"

char ** nodes = 0; 
int num_nodes = -1;

message * (*recv_from)(int, long, int) = 0;
int (*send_to)(int, long, enum message_t, long, long) = 0;

message * recv_from_network(int from_node, long slot, enum message_t expected) {
  return 0;
}

int send_to_network(int node, long ticket, enum message_t type, long slot, long value) {
  return -1;  
}

void init() {
  recv_from = recv_from_network;
  send_to = send_to_network;
}
