#include <intheory.h>
#include <network.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


void test_proposer();

int main(int argc, char **args) {
  test_proposer();
  return 0;
}

long (*recv)[4];
long (*send)[4];

/**
 * PROPOSER SCENARIO: Basic
 **/
long recv1[][4] = { 
  { CLIENT_VALUE, -1, 0, 999},
  { ACCEPTED_PROPOSAL, 1, 0, -1},
  { ACCEPTED_PROPOSAL, 1, 0, -1},
  { ACCEPTED_PROPOSAL, 1, 0, -1}
};

long send1[][4] = {
  { PROPOSAL, 1, 0, 999},
  { PROPOSAL, 1, 0, 999},
  { PROPOSAL, 1, 0, 999},
  { LEARN_THIS, 1, 0, 999},
  { LEARN_THIS, 1, 0, 999},
  { LEARN_THIS, 1, 0, 999},
  { LEARN_THIS, 1, 0, 999}
};

/**
 * PROPOSER SCENARIO: FAILURE OF ACCEPTOR
 **/
long recv2[][4] = { 
  { CLIENT_VALUE, -1, 0, 999},
  { ACCEPTED_PROPOSAL, 1, 0, -1},
  { ACCEPTED_PROPOSAL, 1, 0, -1},
  { -1, -1, -1, -1},
  { ACCEPTED_PROPOSAL, 1, 0, -1},
  { ACCEPTED_PROPOSAL, 1, 0, -1},
  { ACCEPTED_PROPOSAL, 1, 0, -1},
};

long send2[][4] = {
  { PROPOSAL, 1, 0, 999},
  { PROPOSAL, 1, 0, 999},
  { PROPOSAL, 1, 0, 999},
  { PROPOSAL, 1, 0, 999},
  { LEARN_THIS, 1, 0, 999},
  { LEARN_THIS, 1, 0, 999},
  { LEARN_THIS, 1, 0, 999},
  { LEARN_THIS, 1, 0, 999}
};

/**
 * PROPOSER SCENARIO: Acceptor returns value with newer ticket
 */
long recv3[][4] = { 
  { CLIENT_VALUE, -1, 0, 999},
  { ACCEPTED_PROPOSAL, 1, 0, -1},
  { ACCEPTED_PROPOSAL, 1, 0, -1},
  { ACCEPTED_PROPOSAL, 2, 0, 888},
};

long send3[][4] = {
  { PROPOSAL, 1, 0, 999},
  { PROPOSAL, 1, 0, 999},
  { PROPOSAL, 1, 0, 999},
  { LEARN_THIS, 2, 0, 888},
  { LEARN_THIS, 2, 0, 888},
  { LEARN_THIS, 2, 0, 888}
};

int sendidx = 0;
int recvidx = 0;

message * recv_from_scenario(int from_node, long slot, enum message_t expected) {
  if (recv[recvidx][0] == -1) { 
    recvidx++;
    return 0;
  }
  message *msg; 
  msg = malloc(sizeof(message));
  msg->type = recv[recvidx][0];
  msg->ticket = recv[recvidx][1];
  msg->slot = recv[recvidx][2];
  msg->value = recv[recvidx][3];
  printf("< Recv - %d - ticket %ld type %d slot %ld value %ld\n", 
	 recvidx, msg->ticket, msg->type, msg->slot, msg->value);
  recvidx++;  
  return msg;
}

int send_to_scenario(int node, long ticket, enum message_t type, long slot, long value) {
  printf("> Send - %d - ticket %ld type %d slot %ld value %ld\n", 
	 sendidx, ticket, type, slot, value);
  assert(type == send[sendidx][0]);
  assert(ticket == send[sendidx][1]);
  assert(slot == send[sendidx][2]);
  assert(value == send[sendidx][3]);
  sendidx++;
  return 1;
}

void test_proposer() {
  num_nodes = 4;
  recv = recv1;
  send = send1;
  recv_from = &recv_from_scenario; 
  send_to = &send_to_scenario; 
  intheory_sm(LEADER);
  recv = recv2;
  send = send2;
  sendidx = recvidx = 0;
  intheory_sm(LEADER);
  //recv = recv3;
  //send = send3;
  //sendidx = recvidx = 0;
  //sm(LEADER);
} 
