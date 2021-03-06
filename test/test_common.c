#include "../src/include/intheory.h"
#include "../src/include/state_machine.h"
#include "../src/include/proposer.h"
#include "../src/include/acceptor.h"
#include "../src/include/learner.h"
#include "../src/include/network.h"
#include "../src/include/logger.h"
#include "include/test_common.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

long (*recv)[6];
long (*send)[6];
int sendidx = 0;
int recvidx = 0;

/*
 * Run a role to completion
 */
void intheory_sm(enum role_t role) {
  state s;
  
  s = init_state(role, 0);
  do {
    switch(role) {
    case PROPOSER:
      s = sm_proposer(s);
      break;
    case ACCEPTOR:
      s = sm_acceptor(s);
      break;
    case LEARNER:
      s = sm_learner(s);      
      break;  
    case CLIENT:
      break;
    }    
  } while(s.state != S_DONE);
}

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
  msg->flags = recv[recvidx][5];
  //log_message("<<<< RECIEVE ", msg);
  log_graph(from_node, 0, msg->type, 1);
  recvidx++;  
  return msg;
}

int send_to_scenario(int node, long ticket, unsigned int type, long slot, long value, unsigned short flags) {
  trace("GOT      Send - %d - node %d ticket %ld type %s slot %ld value %ld flags %ud", 
  	sendidx, node, ticket, getMessageName(type), slot, value, flags);
  trace("EXPECTED Send - %d - node %d ticket %ld type %s slot %ld value %ld flags %ud", 
  	sendidx, send[sendidx][0], send[sendidx][2], getMessageName(send[sendidx][1]), send[sendidx][3], send[sendidx][4], send[sendidx][5]);
  log_graph(0, node, type, 0);
  if (send[sendidx][0] != -1)
    assert(node == send[sendidx][0]);
  assert(type == send[sendidx][1]);
  assert(ticket == send[sendidx][2]);
  assert(slot == send[sendidx][3]);
  assert(value == send[sendidx][4]);
  assert(flags == send[sendidx][5]);
  sendidx++;
  return 1;
}
