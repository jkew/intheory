#include <../src/include/intheory.h>
#include <include/test_common.h>
#include <stdio.h>
#include <stdlib.h>

void got_value(long slot, long value) {
  running = 0;
  if (value >= 10) {
    sleep(2);
    stop_intheory();
    exit(0);
  }
  if ((value % TOTAL_NODES) == my_id()) {
    set_it(SLOT, value + 1, ASYNC_SEND);
  }
}

int main() {
  set_log_level(NONE);
  char * all_nodes[] = {"127.0.0.1:1235", "127.0.0.1:1236", "127.0.0.1:1237", "127.0.0.1:1238"};
  return spawn_nodes(all_nodes);
}


