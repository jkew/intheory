#include <../src/include/intheory.h>
#include <stdio.h>
#include <stdlib.h>

#define SLOT 0
#define COUNT_TO 10
#define TOTAL_NODES 4

void got_count(long slot, long value) {
  if (value >= 10) {
    stop_intheory();
    exit(0);
  }
  if ((value % TOTAL_NODES) == my_id()) {
    set_it_async(SLOT, value + 1);
  }
}

int main() {
  char * all_nodes[] = {"127.0.0.1:1234", "127.0.0.1:1235", "127.0.0.1:1236", "127.0.0.1:1237"};
  int node = 3;
  int pids[4] = { 0, 0, 0, 0 };

  int pid;

  set_log_level(GRAPH);

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
    register_changed_cb(SLOT, got_count);
    if (node == 0) {
      sleep(2);
      set_it(SLOT, 1);
    }
    while (1) { sleep(1); }
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


