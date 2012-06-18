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

state init_state(enum role_t role);

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
  state s, nextstate;
  s.type = s.node_num = s.nodes_left = s.slot = s.value = -1;
  s.depth = 0; 
  s.state = S_AVAILABLE;
  s.ticket = -1;
  switch(role) {
  case PROPOSER:
    s.ticket = 0;
    return s;
  case ACCEPTOR:
  case LEARNER:
    return s;
    break;  
  case CLIENT:
    break;
  }
}

void next_state(enum role_t role) {
  state s;
  switch(role) {
  case PROPOSER:
    s = saved_proposer;
    s = sm_proposer(s);
    if (s.state == S_DONE)  {
      saved_proposer = init_state(PROPOSER);
    } else {
      saved_proposer = s;
    }
    return;
  case ACCEPTOR:
    s = saved_acceptor;
    s = sm_acceptor(s);
    if (s.state == S_DONE)  {
      saved_acceptor = init_state(ACCEPTOR);
    } else {
      saved_acceptor = s;
    }
    return;
  case LEARNER:
    s = saved_learner;
    s = sm_learner(s);
    if (s.state == S_DONE)  {
      saved_learner = init_state(LEARNER);
    } else {
      saved_learner = s;
    }
    return;
  case CLIENT:
    break;
  }
}

/**
 * For testing only
 */ 
void intheory_sm(enum role_t role) {
  state s, nextstate;
  s.type = s.node_num = s.nodes_left = s.slot = s.value = -1;
  s.depth = 0; 
  s.state = S_AVAILABLE;
  s.ticket = -1;
  switch(role) {
  case PROPOSER:
    s.ticket = 0;
    do {
      nextstate = sm_proposer(s);
    } while(nextstate.state != S_DONE);
    break;
  case ACCEPTOR:
    do {
      nextstate = sm_acceptor(s);
    } while(nextstate.state != S_DONE);
    break;
  case LEARNER:
    do {
      nextstate = sm_learner(s);
    } while(nextstate.state != S_DONE);
    break;  
  case CLIENT:
    break;
  }
}

