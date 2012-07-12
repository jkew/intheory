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
#include "include/store.h"


int running = 0;
unsigned long deadline = 10000;

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
    if (saved_proposer.state == S_AVAILABLE
	&& saved_acceptor.state == S_AVAILABLE
	&& saved_learner.state == S_AVAILABLE) {
      usleep(5000);
    }
  }
}

void start_intheory(int my_index, int node_count, char* nodes[]) {
  assert(my_index < node_count);
  running = 1;  

  saved_learner = init_state(LEARNER, 0);
  saved_proposer = init_state(PROPOSER, 0);
  saved_acceptor = init_state(ACCEPTOR, 0);

  // initialize the network
  init_store();
  notice("INTIALIZED STORE");
  init_network(my_index, node_count, nodes, 256);
  notice("INTIALIZED NETWORK");
  // start the server
  start_server();
  
  // start the worker
  pthread_create(&worker_thread, 0, worker, 0);

  notice("INTHEORY STARTED, nodes: %d", num_nodes());
}

void stop_intheory() {
  running = 0;
  trace("Stopping worker");
  pthread_join(&worker_thread, 0);
  trace("Stopping network server");
  stop_server();
  trace("Destroying network datastructures");
  destroy_network();
  trace("Destroying slot store");
  destroy_store();
  trace("Destroying callback datastructures");
  destroy_cb();
  notice("INTHEORY STOPPED");
}


int set_it(long slot, long value) {
  send_to(my_id(), -1, CLIENT_VALUE, slot, value);
  message *msg = 0; 
  while ((msg = recv_from(CLIENT, -1, slot, WRITE_SUCCESS | WRITE_FAILED)) == 0) {
    sleep(1);
  }
  int ret = 0;
  if (msg->type == WRITE_SUCCESS) {
    ret = 1;
  } else {
    discard(msg);
  }
  return ret;
}

void set_it_async(long slot, long value) {
  send_to(my_id(), -1, CLIENT_VALUE, slot, value);
}

int get_it(long slot, long *value) {
  send_to(my_id(), -1, GET, slot, -1);
  message *msg = 0;
  int tries = 10;
  while ((msg = recv_from(CLIENT, -1, slot, READ_SUCCESS | READ_FAILED)) == 0 && tries--) {
    sleep(1);
  }
  int ret = 0;
  if (msg != 0 && msg->type == READ_SUCCESS) {
    ret = 1;
    *value = msg->value;
  } 
  discard(msg);
  return ret;
}

// TODO move to state_machine.c, and more complicated case to learner/proposer
state init_state(enum role_t role, state *prev_state) {
  state s;
  s.type = s.num_quorom = s.max_fails = s.client = s.nodes_left = s.slot = s.value = s.ticket =-1;
  s.depth = s.fails = 0; 
  s.state = S_AVAILABLE;
  switch(role) {
  case LEARNER:
    if (prev_state != 0) {
      s.slot = prev_state->slot;
      s.value = prev_state->value;
    }
  case PROPOSER:
    if (prev_state != 0) 
      s.ticket = prev_state->ticket;
    else
      s.ticket = 0;
  case ACCEPTOR:
  case CLIENT:
    break;
  }
  return s;
}

//TODO move to state_machine
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
    new_state = init_state(role, &new_state);
  }
  *s = new_state;
  return;
}

// TODO Move to state_machine
/**
 * For testing only
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

