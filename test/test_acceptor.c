#include "../src/include/intheory.h"
#include "../src/include/state_machine.h"
#include "../src/include/network.h"
#include "include/test_common.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


void test_acceptor();

int main(int argc, char **args) {
  test_acceptor();
  return 0;
}

/**
 * ACCEPTOR SCENARIO: Basic accept and set
 **/
long acceptor_basic_recv[][6] = { 
  { 1, PROPOSAL, 1, 0, 999, 0},
  { 1, ACCEPTOR_SET, 1, 0, 999, 0}
};

long acceptor_basic_send[][6] = {
  { 1, ACCEPTED_PROPOSAL, 1, 0, 999, 0},
  { -1, SET, 1, 0, 999, 0},
  { -1, SET, 1, 0, 999, 0},
  { -1, SET, 1, 0, 999, 0},
  { -1, SET, 1, 0, 999, 0},
};

/**
 * ACCEPTOR SCENARIO: Basic received higher value from another proposer
 **/
long acceptor_high_recv[][6] = { 
  { 1, PROPOSAL, 1, 0, 999, 0},
  { 2, PROPOSAL, 5, 0, 888, 0},
  { 1, ACCEPTOR_SET, 1, 0, 999, 0},
  { 2, ACCEPTOR_SET, 5, 0, 999, 0}
};

long acceptor_high_send[][6] = {
  { 1, ACCEPTED_PROPOSAL, 1, 0, 999, 0},
  { 2, ACCEPTED_PROPOSAL, 5, 0, 888, 0},
  { -1, SET, 5, 0, 888, 0},
  { -1, SET, 5, 0, 888, 0},
  { -1, SET, 5, 0, 888, 0},
  { -1, SET, 5, 0, 888, 0},
};


/**
 * ACCEPTOR SCENARIO: Proposer fails
 **/
long acceptor_pfail_recv[][6] = { 
  { 1, PROPOSAL, 1, 0, 999, 0},
  { -1, -1, -1, -1, -1, 0},
  { 2, PROPOSAL, 2, 0, 777, 0},
  { 2, ACCEPTOR_SET, 2, 0, 777, 0}
};

long acceptor_pfail_send[][6] = {
  { 1, ACCEPTED_PROPOSAL, 1, 0, 999, 0},
  { 2, ACCEPTED_PROPOSAL, 2, 0,777, 0},
  { -1, SET, 2, 0, 777, 0},
  { -1, SET, 2, 0, 777, 0},
  { -1, SET, 2, 0, 777, 0},
  { -1, SET, 2, 0, 777, 0},
};

/**
 * ACCEPTOR SCENARIO: Basic received lower value from another proposer
 **/
long acceptor_lower_recv[][6] = { 
  { 1, PROPOSAL, 5, 0, 999, 0},
  { 2, PROPOSAL, 3, 0, 666, 0},
  { 1, ACCEPTOR_SET, 5, 0, 999, 0}
};

long acceptor_lower_send[][6] = {
  { 1, ACCEPTED_PROPOSAL, 5, 0, 999, 0},
  { 2, REJECTED_PROPOSAL, 5, 0, 999, 0},
  { -1, SET, 5, 0, 999, 0},
  { -1, SET, 5, 0, 999, 0},
  { -1, SET, 5, 0, 999, 0},
  { -1, SET, 5, 0, 999, 0},
};

void test_acceptor() {
  set_log_level(NONE);
  _node_count = 4;
  recv_from = &recv_from_scenario; 
  send_to = &send_to_scenario;
  
  sendidx = recvidx = 0;
  recv = acceptor_basic_recv;
  send = acceptor_basic_send;
  intheory_sm(ACCEPTOR);

  sendidx = recvidx = 0;
  recv = acceptor_high_recv;
  send = acceptor_high_send;
  intheory_sm(ACCEPTOR);
  
  sendidx = recvidx = 0;
  recv = acceptor_pfail_recv;
  send = acceptor_pfail_send;
  intheory_sm(ACCEPTOR);

  sendidx = recvidx = 0;
  recv = acceptor_lower_recv;
  send = acceptor_lower_send;
  intheory_sm(ACCEPTOR);
}
