#include <../src/include/intheory.h>
#include <include/test_common.h>
#include <stdio.h>
#include <stdlib.h>

void got_value(int slot, long value, unsigned short op) {
  if (value >= COUNT_TO) {
    sleep(2);
    stop_intheory();
    running = 0;
    printf("TEST_4NODE_COUNT_SUCCESS!");
    exit(0);
  }
  if ((value % TOTAL_NODES) == my_id()) {
    set_it(SLOT, value + 1, ASYNC_SEND);
  }
}

void loop() {
  while (running) { sleep(1); }
}

int main() {
  set_log_level(GRAPH);
  char * all_nodes[] = {"A", "B", "C", "D"};
  return spawn_nodes(all_nodes);
}


