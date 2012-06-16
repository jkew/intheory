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

void intheory_worker() {
  while(running) {
    intheory_sm(PROPOSER);
    intheory_sm(ACCEPTOR);
    intheory_sm(LEARNER);
  }
}

void start_intheory(int port, int other_node_count, char* other_nodes[]) {
  running = 1;
  // munge these together
  char * all_nodes[other_node_count + 1];
  char me[256];
  sprintf(me, "localhost:%d", port);
  all_nodes[0] = me;
  int i;
  for (i = 1; i <= other_node_count; i++) {
    all_nodes[i] = other_nodes[i - 1];
  }

  // initialize the network
  init_network(other_node_count + 1, all_nodes, 256);
  
  // start the server
  start_server();
  
  // start the worker
  pthread_create(&worker_thread, 0, intheory_worker, 0);
  printf("INTHEORY STARTED, nodes: %d\n", num_nodes);
}

void stop_intheory() {
  running = 0;
  stop_server();
  pthread_join(&worker_thread, 0);
  destroy_network();
  printf("INTHEORY STOPPED\n");
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

