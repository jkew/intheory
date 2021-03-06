#include "../src/include/intheory.h"
#include "../src/include/state_machine.h"
#include "../src/include/network.h"
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
long proposer_basic_recv[][6] = { 
  { 3, CLIENT_VALUE, -1, 0, 999, 0},
  { -1, ACCEPTED_PROPOSAL, 1, 0, -1, 0},
  { -1, ACCEPTED_PROPOSAL, 1, 0, -1, 0},
  { -1, ACCEPTED_PROPOSAL, 1, 0, -1, 0},
  { -1, ACCEPTED_PROPOSAL, 1, 0, -1, 0},
};

long proposer_basic_send[][6] = {
  { -1, PROPOSAL, 1, 0, 999, 0},
  { -1, PROPOSAL, 1, 0, 999, 0},
  { -1, PROPOSAL, 1, 0, 999, 0},
  { -1, PROPOSAL, 1, 0, 999, 0},
  { -1, ACCEPTOR_SET, 1, 0, 999, 0},
  { -1, ACCEPTOR_SET, 1, 0, 999, 0},
  { -1, ACCEPTOR_SET, 1, 0, 999, 0},
  { -1, ACCEPTOR_SET, 1, 0, 999, 0},
  { 3, WRITE_SUCCESS, 1, 0, 999, 0},
};

/**
 * PROPOSER SCENARIO: FAILURE OF ACCEPTOR
 **/
long proposer_acceptor_fails_recv[][6] = { 
  { 3, CLIENT_VALUE, -1, 0, 999, 0},
  { -1, ACCEPTED_PROPOSAL, 1, 0, -1, 0},
  { -1, ACCEPTED_PROPOSAL, 1, 0, -1, 0},
  { -1, ACCEPTED_PROPOSAL, 1, 0, -1, 0},
  { -1, -1, -1, -1, -1, 0},
  { -1, ACCEPTED_PROPOSAL, 1, 0, -1, 0},
  { -1, ACCEPTED_PROPOSAL, 1, 0, -1, 0},
  { -1, ACCEPTED_PROPOSAL, 1, 0, -1, 0},
  { -1, ACCEPTED_PROPOSAL, 1, 0, -1, 0},
};

long proposer_acceptor_fails_send[][6] = {
  { -1, PROPOSAL, 1, 0, 999, 0},
  { -1, PROPOSAL, 1, 0, 999, 0},
  { -1, PROPOSAL, 1, 0, 999, 0},
  { -1, PROPOSAL, 1, 0, 999, 0},
  { -1, ACCEPTOR_SET, 1, 0, 999, 0},
  { -1, ACCEPTOR_SET, 1, 0, 999, 0},
  { -1, ACCEPTOR_SET, 1, 0, 999, 0},
  { 3, WRITE_SUCCESS, 1, 0, 999, 0},
};

/**
 * PROPOSER SCENARIO: Acceptor returns value with newer ticket
 */
long proposer_acceptor_has_new_ticket_recv[][6] = { 
  { 3, CLIENT_VALUE, -1, 0, 999, 0},
  { -1, ACCEPTED_PROPOSAL, 1, 0, -1, 0},
  { -1, ACCEPTED_PROPOSAL, 1, 0, -1, 0},
  { -1, ACCEPTED_PROPOSAL, 1, 0, -1, 0},
  { -1, ACCEPTED_PROPOSAL, 2, 0, 888, 0},
  { -1, ACCEPTED_PROPOSAL, 3, 0, 888, 0},
  { -1, ACCEPTED_PROPOSAL, 3, 0, 888, 0},
  { -1, ACCEPTED_PROPOSAL, 3, 0, 888, 0},
  { -1, ACCEPTED_PROPOSAL, 3, 0, 888, 0},
};

long proposer_acceptor_has_new_ticket_send[][6] = {
  { -1, PROPOSAL, 1, 0, 999, 0},
  { -1, PROPOSAL, 1, 0, 999, 0},
  { -1, PROPOSAL, 1, 0, 999, 0},
  { -1, PROPOSAL, 1, 0, 999, 0},
  { -1, PROPOSAL, 3, 0, 888, 0},
  { -1, PROPOSAL, 3, 0, 888, 0},
  { -1, PROPOSAL, 3, 0, 888, 0},
  { -1, PROPOSAL, 3, 0, 888, 0},
  { -1, ACCEPTOR_SET, 3, 0, 888, 0},
  { -1, ACCEPTOR_SET, 3, 0, 888, 0},
  { -1, ACCEPTOR_SET, 3, 0, 888, 0},
  { -1, ACCEPTOR_SET, 3, 0, 888, 0},
  { 3, WRITE_SUCCESS, 3, 0, 888, 0},
};

/**
 * PROPOSER SCENARIO: Nothing responds
 */
long proposer_dead_inside_recv[][6] = { 
  { 3, CLIENT_VALUE, -1, 0, 999, 0},
  { -1, -1, -1, -1, -1, 0},  { -1, -1, -1, -1, -1, 0},
  { -1, -1, -1, -1, -1, 0},  { -1, -1, -1, -1, -1, 0},
  { -1, -1, -1, -1, -1, 0},  { -1, -1, -1, -1, -1, 0},
  { -1, -1, -1, -1, -1, 0},  { -1, -1, -1, -1, -1, 0},
  { -1, -1, -1, -1, -1, 0},  { -1, -1, -1, -1, -1, 0},
  { -1, -1, -1, -1, -1, 0},  { -1, -1, -1, -1, -1, 0},
  { -1, -1, -1, -1, -1, 0},  { -1, -1, -1, -1, -1, 0},
};

long proposer_dead_inside_send[][6] = {
  { -1, PROPOSAL, 1, 0, 999, 0},
  { -1, PROPOSAL, 1, 0, 999, 0},
  { -1, PROPOSAL, 1, 0, 999, 0},
  { -1, PROPOSAL, 1, 0, 999, 0},
  { 3, WRITE_FAILED, 1, 0, 999, 0},
};

void test_proposer() {
  set_log_level(NONE);
  deadline = -1;
  _node_count = 4;
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

  recv = proposer_dead_inside_recv;
  send = proposer_dead_inside_send;
  sendidx = recvidx = 0;
  intheory_sm(PROPOSER);  
} 
