#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include "include/util.h"
#include "include/state_machine.h"
#include "include/intheory.h"
#include "include/network.h"
#include "include/proposer.h"
#include "include/acceptor.h"
#include "include/learner.h"

int running = 0;

// get rid of these
pthread_t proposer_thread;
pthread_t acceptor_thread;
pthread_t learner_thread;

state saved_proposer;
state saved_acceptor;
state saved_learner;

void sm_init() {
  state s;
  s.type = s.node_num = s.nodes_left = s.slot = s.value = -1;
  s.depth = 0; 
  s.state = S_AVAILABLE;
  s.ticket = -1;
  saved_acceptor = s;  
  saved_learner = s;
  s.ticket = 0;
  saved_proposer = s;
}

void proposer_worker(void *args) {
  error("propser started");
  while(running) {
    intheory_sm(PROPOSER);
    usleep(100000);
  }
}



void acceptor_worker(void *args) {
  error("acceptor started");
  while(running) {
    intheory_sm(ACCEPTOR);
    usleep(100000);
  }
}


void learner_worker(void *args) {
  error("learner started");
  while(running) {
    intheory_sm(LEARNER);
    usleep(100000);
  }
}

void start_intheory(char *me, int other_node_count, char* other_nodes[]) {
  running = 1;
  // munge these together
  char * all_nodes[other_node_count + 1];
  all_nodes[0] = me;
  int i;
  for (i = 1; i <= other_node_count; i++) {
    all_nodes[i] = other_nodes[i - 1];
  }

  sm_init();

  // initialize the network
  init_network(other_node_count + 1, all_nodes, 256);
  
  // start the server
  start_server();
  
  // start the worker
  // TODO: instead of using three threads, use an interruptable
  // state machine with yeild
  pthread_create(&proposer_thread, 0, proposer_worker, 0);
  pthread_create(&acceptor_thread, 0, acceptor_worker, 0);
  pthread_create(&learner_thread, 0, learner_worker, 0);

  info("INTHEORY STARTED, nodes: %d", num_nodes);
}

void stop_intheory() {
  running = 0;
  stop_server();
  pthread_join(&proposer_thread, 0);
  pthread_join(&acceptor_thread, 0);
  pthread_join(&learner_thread, 0);
  destroy_network();
  info("INTHEORY STOPPED");
}


int set_it(long slot, long value) {
  send_to(my_id(), -1, CLIENT_VALUE, slot, value);
  message *msg = recv_from(CLIENT, -1, slot, WRITE_SUCCESS | WRITE_FAILED);
  int ret = 0;
  if (msg != 0 && msg->type == WRITE_SUCCESS) {
    ret = 1;
  } 
  discard(msg);
  return ret;
}

int get_it(long slot, long *value) {
  send_to(my_id(), -1, GET, slot, -1);
  message *msg = recv_from(CLIENT, -1, slot, READ_SUCCESS | READ_FAILED);
  int ret = 0;
  if (msg != 0 && msg->type == READ_SUCCESS) {
    ret = 1;
    *value = msg->value;
  } 
  discard(msg);
  return ret;
}


// TODO; we want to make these state machines interruptable,
// which means exiting the stack asap for a restart later.
void yeild(enum role_t role, state s) {
  if (role == PROPOSER) {
    saved_proposer = s;
  }
  if (role == ACCEPTOR) {
    saved_acceptor = s;
  }
  if (role == LEARNER) {
    saved_learner = s;
  }
}

void intheory_sm(enum role_t role) {
  state s;
  s.type = s.node_num = s.nodes_left = s.slot = s.value = -1;
  s.depth = 0; 
  s.state = S_AVAILABLE;
  s.ticket = -1;
  switch(role) {
  case PROPOSER:
    s.ticket = 0;
    sm_proposer(s);
    break;
  case ACCEPTOR:
    sm_acceptor(s);
    break;
  case LEARNER:
    sm_learner(s);
    break;  
  case CLIENT:
    break;
  }
}

