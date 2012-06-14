#include "../../main/c/include/intheory.h"
#include "../../main/c/include/network.h"
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

void test_acceptor() {
  num_nodes = 4;
  sendidx = recvidx = 0;
  recv_from = &recv_from_scenario; 
  send_to = &send_to_scenario;
  recv = acceptor_basic_recv;
  send = acceptor_basic_send;
  intheory_sm(ACCEPTOR);
}
