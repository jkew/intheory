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

pthread_t worker_thread;


// TODO: Ideally we should have one state per slot
state saved_proposer;
state saved_acceptor;
state saved_learner;

// TODO: May be able to eventually merge this with the server thread
// and register callbacks when specific messages are received
void worker(void *args) {
  notice("intheory worker thread started");
  while(running) {
    next_state(PROPOSER);
    next_state(ACCEPTOR);
    next_state(LEARNER);
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

  saved_learner = init_state(LEARNER);
  saved_proposer = init_state(PROPOSER);
  saved_acceptor = init_state(ACCEPTOR);

  // initialize the network
  init_network(other_node_count + 1, all_nodes, 256);
  
  // start the server
  start_server();
  
  // start the worker
  pthread_create(&worker_thread, 0, worker, 0);

  notice("INTHEORY STARTED, nodes: %d", num_nodes);
}

void stop_intheory() {
  running = 0;
  stop_server();
  pthread_join(&worker_thread, 0);
  destroy_network();
  notice("INTHEORY STOPPED");
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

state init_state(enum role_t role) {
  state s;
  s.type = s.num_quorom = s.max_fails = s.client = s.nodes_left = s.slot = s.value = -1;
  s.depth = s.fails = 0; 
  s.state = S_AVAILABLE;
  s.ticket = -1;
  switch(role) {
  case PROPOSER:
    s.ticket = 0;
  case ACCEPTOR:
  case LEARNER:
  case CLIENT:
    break;
  }
  return s;
}

void next_state(enum role_t role) {
  state *s;
  sm_role_fn sm = 0;
  switch(role) {
  case PROPOSER:
    s = &saved_proposer;
    sm = sm_proposer;
    break;
  case ACCEPTOR:
    s = &saved_acceptor;
    sm = sm_acceptor;
    break;
  case LEARNER:
    s = &saved_learner;
    sm = sm_learner;
    break;
  case CLIENT:
    assert(0);
    break;
  }
  state new_state = sm(*s);

  if (new_state.state == S_DONE) {
    *s = init_state(role);
  } else {
    *s = new_state;
  }
  return;
}

/**
 * For testing only
 */ 
void intheory_sm(enum role_t role) {
  state s;
  
  s = init_state(role);
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

