#include "../src/include/intheory.h"
#include "../src/include/network.h"
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
long learner_basic_recv[][5] = { 
  { 1, SET, 1, 0, 999},
  { 2, SET, 1, 0, 999},
  { 3, SET, 1, 0, 999},
  { 4, SET, 1, 0, 999},
  { -1, -1, -1, -1, -1},
  { 66, GET, -1, 0, -1},
};

long learner_basic_send[][5] = {
  { 66, READ_SUCCESS, -1, 0, 999}
};

/**
 * LEARNER SCENARIO: get fails
 **/
long learner_getfail_recv[][5] = { 
  { 66, GET, -1, 700, -1},
  { -1, -1, -1, -1, -1}
};

long learner_getfail_send[][5] = {
  { 66, READ_FAILED, -1, 700, -1}
};

/**
 * LEARNER SCENARIO: expand slots
 **/
long learner_expand_recv[][5] = { 
  { 1, SET, 1, 777, 777},
  { 2, SET, 1, 777, 777},
  { 3, SET, 1, 777, 777},
  { 4, SET, 1, 777, 777},
  { -1, -1, -1, -1, -1},
  { 66, GET, -1, 777, -1},
};

long learner_expand_send[][5] = {
  { 66, READ_SUCCESS, -1, 777, 777}
};

// INVALID SET
long learner_mixed_recv[][5] = { 
  { 1, SET, 2, 0, 999},
  { 666, SET, 1, 0, 777},
  { 2, SET, 2, 0, 999},
  { 667, SET, 1, 0, 777},
  { 3, SET, 2, 0, 999},
  { 668, SET, 1, 0, 777},
  { 4, SET, 1, 0, 999},
  { 669, SET, 1, 0, 777},
  { -1, -1, -1, -1, -1},
  { 66, GET, -1, 0, -1},
};


long learner_mixed_send[][5] = {
  { 66, READ_SUCCESS, -1, 0, 777}
};

long learner_mixed2_recv[][5] = { 
  { 666, SET, 1, 0, 777},
  { 1, SET, 2, 0, 999},
  { 667, SET, 1, 0, 777},
  { 2, SET, 2, 0, 999},
  { 668, SET, 1, 0, 777},
  { 3, SET, 2, 0, 999},
  { 669, SET, 1, 0, 777},
  { 4, SET, 1, 0, 999},
  { -1, -1, -1, -1, -1},
  { 66, GET, -1, 0, -1},
};

long learner_mixed2_send[][5] = {
  { 66, READ_SUCCESS, -1, 0, 777}
};

/**
 * Only the quorum size was received
 */
long learner_some_recv[][5] = { 
  { 1, SET, 1, 0, 999},
  { 2, SET, 1, 0, 999},
  { 3, SET, 1, 0, 999},
  { -1, -1, -1, -1, -1},
  { 66, GET, -1, 0, -1},
};

long learner_some_send[][5] = {
  { 66, READ_SUCCESS, -1, 0, 999}
};
void test_learner() {
  num_nodes = 4;
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
  sendidx = recvidx = 0;
  recv = learner_mixed2_recv;
  send = learner_mixed2_send;
  intheory_sm(LEARNER);
  sendidx = recvidx = 0;
  recv = learner_some_recv;
  send = learner_some_send;
  intheory_sm(LEARNER);
}
