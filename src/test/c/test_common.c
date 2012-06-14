#include "../../main/c/include/intheory.h"
#include "../../main/c/include/network.h"
#include "include/test_common.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

long (*recv)[5];
long (*send)[5];
int sendidx = 0;
int recvidx = 0;

message * recv_from_scenario(int from_node, long slot, unsigned int mask) {
  // handle a failure to receive within a 'time limit'
  if (recv[recvidx][1] == -1) { 
    recvidx++;
    return 0;
  }
  message *msg; 
  msg = malloc(sizeof(message));
  if (recv[recvidx][0] == -1)
    msg->from = from_node;
  else
    msg->from = recv[recvidx][0];
  msg->type = recv[recvidx][1];
  msg->ticket = recv[recvidx][2];
  msg->slot = recv[recvidx][3];
  msg->value = recv[recvidx][4];

  printf("< Recv - %d - ticket %ld type %d slot %ld value %ld\n", 
	 recvidx, msg->ticket, msg->type, msg->slot, msg->value);
  recvidx++;  
  return msg;
}

int send_to_scenario(int node, long ticket, unsigned int type, long slot, long value) {
  printf("> Send - %d - node %d ticket %ld type %d slot %ld value %ld\n", 
	 sendidx, node, ticket, type, slot, value);
  if (send[sendidx][0] != -1)
    assert(node == send[sendidx][0]);
  assert(type == send[sendidx][1]);
  assert(ticket == send[sendidx][2]);
  assert(slot == send[sendidx][3]);
  assert(value == send[sendidx][4]);
  sendidx++;
  return 1;
}
