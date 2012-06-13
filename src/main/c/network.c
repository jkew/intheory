#include "include/network.h"

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
