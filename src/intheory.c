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
#include "include/state_machine.h"

int running = 0;
unsigned long deadline = 10000;
pthread_t worker_thread;

// TODO: May be able to eventually merge this with the server thread
// and register callbacks when specific messages are received
void worker(void *args) {
  notice("intheory worker thread started");
  while(running) {
    next_states();
    usleep(5000);
  }
}

void start_intheory(int my_index, int node_count, char* nodes[]) {
  assert(my_index < node_count);
  running = 1;  
  init_sm();
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
  } 
  discard(msg);
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


