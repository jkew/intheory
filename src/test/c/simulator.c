#include "../../main/c/include/intheory.h"
#include "../../main/c/include/network.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


void test_proposer();

int main(int argc, char **args) {
  test_proposer();
  test_acceptor();
  return 0;
}

long (*recv)[5];
long (*send)[5];

/**
 * PROPOSER SCENARIO: Basic
 **/
long proposer_basic_recv[][5] = { 
  { -1, CLIENT_VALUE, -1, 0, 999},
  { -1, ACCEPTED_PROPOSAL, 1, 0, -1},
  { -1, ACCEPTED_PROPOSAL, 1, 0, -1},
  { -1, ACCEPTED_PROPOSAL, 1, 0, -1}
};

long proposer_basic_send[][5] = {
  { -1, PROPOSAL, 1, 0, 999},
  { -1, PROPOSAL, 1, 0, 999},
  { -1, PROPOSAL, 1, 0, 999},
  { -1, SET, 1, 0, 999},
  { -1, SET, 1, 0, 999},
  { -1, SET, 1, 0, 999},
  { -1, SET, 1, 0, 999}
};

/**
 * PROPOSER SCENARIO: FAILURE OF ACCEPTOR
 **/
long proposer_acceptor_fails_recv[][5] = { 
  { -1, CLIENT_VALUE, -1, 0, 999},
  { -1, ACCEPTED_PROPOSAL, 1, 0, -1},
  { -1, ACCEPTED_PROPOSAL, 1, 0, -1},
  { -1, -1, -1, -1, -1},
  { -1, ACCEPTED_PROPOSAL, 1, 0, -1},
  { -1, ACCEPTED_PROPOSAL, 1, 0, -1},
  { -1, ACCEPTED_PROPOSAL, 1, 0, -1},
};

long proposer_acceptor_fails_send[][5] = {
  { -1, PROPOSAL, 1, 0, 999},
  { -1, PROPOSAL, 1, 0, 999},
  { -1, PROPOSAL, 1, 0, 999},
  { -1, PROPOSAL, 1, 0, 999},
  { -1, SET, 1, 0, 999},
  { -1, SET, 1, 0, 999},
  { -1, SET, 1, 0, 999},
  { -1, SET, 1, 0, 999}
};

/**
 * PROPOSER SCENARIO: Acceptor returns value with newer ticket
 */
long proposer_acceptor_has_new_ticket_recv[][5] = { 
  { -1, CLIENT_VALUE, -1, 0, 999},
  { -1, ACCEPTED_PROPOSAL, 1, 0, -1},
  { -1, ACCEPTED_PROPOSAL, 1, 0, -1},
  { -1, ACCEPTED_PROPOSAL, 2, 0, 888},
};

long proposer_acceptor_has_new_ticket_send[][5] = {
  { -1, PROPOSAL, 1, 0, 999},
  { -1, PROPOSAL, 1, 0, 999},
  { -1, PROPOSAL, 1, 0, 999},
  { -1, SET, 2, 0, 888},
  { -1, SET, 2, 0, 888},
  { -1, SET, 2, 0, 888}
};

/**
 * ACCEPTOR SCENARIO: Basic accept and set
 **/
long acceptor_basic_recv[][5] = { 
  { 1, PROPOSAL, 1, 0, 999},
  { 1, SET, 1, 0, 999}
};

long acceptor_basic_send[][5] = {
  { 1, ACCEPTED_PROPOSAL, 1, 0, 999},
  { -1, SET, 1, 0, 999},
  { -1, SET, 1, 0, 999},
  { -1, SET, 1, 0, 999},
  { -1, SET, 1, 0, 999},
};

/**
 * ACCEPTOR SCENARIO: Basic received higher value from another proposer
 **/

/**
 * ACCEPTOR SCENARIO: Proposer fails
 **/

/**
 * ACCEPTOR SCENARIO: Learner fails
 **/

/**
 * ACCEPTOR SCENARIO: Basic received lower value from another proposer
 **/

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

void test_proposer() {
  num_nodes = 4;
  recv_from = &recv_from_scenario; 
  send_to = &send_to_scenario; 
  recv = proposer_basic_recv;
  send = proposer_basic_send;
  intheory_sm(PROPOSER);
  recv = proposer_acceptor_fails_recv;
  send = proposer_acceptor_fails_send;
  sendidx = recvidx = 0;
  intheory_sm(PROPOSER);
  recv = proposer_acceptor_has_new_ticket_recv;
  send = proposer_acceptor_has_new_ticket_send;
  sendidx = recvidx = 0;
  intheory_sm(PROPOSER);
} 


void test_acceptor() {
  num_nodes = 4;
  sendidx = recvidx = 0;
  recv_from = &recv_from_scenario; 
  send_to = &send_to_scenario;
  recv = acceptor_basic_recv;
  send = acceptor_basic_send;
  intheory_sm(ACCEPTOR);
}
