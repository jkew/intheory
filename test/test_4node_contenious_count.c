#include <../src/include/intheory.h>
#include <include/test_common.h>
#include <stdio.h>
#include <stdlib.h>

void got_value(long slot, long value) {
  if (!running) return;
  if (value >= 10) {
    running = 0;
    // wait a bit for the nodes to stop screaming
    // and then shutdown
    sleep(2);
    stop_intheory();
    exit(0);
  }
  set_it_async(SLOT, value + 1);
}

int main() {
  set_log_level(NONE);
  char * all_nodes[] = {"127.0.0.1:1239", "127.0.0.1:1240", "127.0.0.1:1241", "127.0.0.1:1242"};
  return spawn_nodes(all_nodes);
}


