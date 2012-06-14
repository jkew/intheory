#include "../../main/c/include/intheory.h"
#include "../../main/c/include/network.h"
#include "include/test_common.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


void test_proposer();

int main(int argc, char **args) {
  test_proposer();
  return 0;
}

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
