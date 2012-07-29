#include "../src/include/intheory.h"
#include "../src/include/state_machine.h"
#include "../src/include/network.h"
#include "../src/include/store.h"
#include "include/test_common.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


void test_learner();

int main(int argc, char **args) {
  test_learner();
  return 0;
}

/**
 * LEARNER SCENARIO: Basic set/get
 **/
long learner_basic_recv[][6] = { 
  { 1, SET, 1, 0, 999, 0},
  { 2, SET, 1, 0, 999, 0},
  { 3, SET, 1, 0, 999, 0},
  { 0, SET, 1, 0, 999, 0},
  { -1, -1, -1, -1, -1, 0},
  { 0, GET, -1, 0, -1, 0},
};

long learner_basic_send[][6] = {
  { 0, READ_SUCCESS, -1, 0, 999, 0}
};

/**
 * LEARNER SCENARIO: get fails
 **/
long learner_getfail_recv[][6] = { 
  { 1, GET, -1, 700, -1, 0},
};

long learner_getfail_send[][6] = {
  { 1, READ_FAILED, -1, 700, -1, 0}
};

/**
 * LEARNER SCENARIO: expand slots
 **/
long learner_expand_recv[][6] = { 
  { 1, SET, 1, 777, 777, 0},
  { 2, SET, 1, 777, 777, 0},
  { 3, SET, 1, 777, 777, 0},
  { 0, SET, 1, 777, 777, 0},
  { -1, -1, -1, -1, -1, 0},
  { 0, GET, -1, 777, -1, 0},
};

long learner_expand_send[][6] = {
  { 0, READ_SUCCESS, -1, 777, 777, 0}
};

long learner_mixed_recv[][6] = { 
  { 1, SET, 2, 0, 999, 0},
  { 2, SET, 1, 0, 777, 0},
  { 1, SET, 2, 0, 999, 0},
  { 2, SET, 1, 0, 777, 0},
  { 1, SET, 2, 0, 999, 0},
  { 2, SET, 1, 0, 777, 0},
  { 1, SET, 1, 0, 999, 0},
  { 2, SET, 1, 0, 777, 0},
  { -1, -1, -1, -1, -1, 0},
  { 0, GET, -1, 0, -1, 0},
};


long learner_mixed_send[][6] = {
  { 0, READ_SUCCESS, -1, 0, 777, 0}
};

void test_learner() {
  
  set_log_level(NONE);
  init_store();

  _node_count = 4;
  recv_from = &recv_from_scenario; 
  send_to = &send_to_scenario;
  sendidx = recvidx = 0;
  recv = learner_basic_recv;
  send = learner_basic_send;
  intheory_sm(LEARNER);

  sendidx = recvidx = 0;
  recv = learner_getfail_recv;
  send = learner_getfail_send;
  intheory_sm(LEARNER);

  sendidx = recvidx = 0;
  recv = learner_expand_recv;
  send = learner_expand_send;
  intheory_sm(LEARNER);

  sendidx = recvidx = 0;
  recv = learner_mixed_recv;
  send = learner_mixed_send;
  intheory_sm(LEARNER);

}
