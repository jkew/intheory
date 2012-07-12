#include <../src/include/intheory.h>
#include <include/test_common.h>
#include <stdio.h>
#include <stdlib.h>

int running = 1;

int spawn_nodes(char *all_nodes) {
  int node = TOTAL_NODES - 1;
  int pids[TOTAL_NODES] = { 0, 0, 0, 0 };

  int pid;

  while (node >= 0 && (pid = fork())) {
    node--;
    if (pid == 0) {      
      break;
    }
    if (pid < 0) {
      exit(1);
    }    
    pids[node] = pid;
    pid = -1;
  } 

  if (pid == 0) {
    start_intheory(node, TOTAL_NODES, all_nodes);
    register_changed_cb(SLOT, got_value);
    if (node == 0) {
      sleep(2);
      set_it(SLOT, 1);
    }
    while (running) { sleep(1); }
  } else {

    int allswellthat = 0;
    node = 3;
    while (node >= 0) {
      int status;
      pid = wait(&status);
      if ( ! WIFEXITED(status) ) allswellthat = 1;
      node--;
    }
    exit(allswellthat);
  }
  exit(0);
}


