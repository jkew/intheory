#include <../src/include/intheory.h>
#include <include/test_common.h>
#include <stdio.h>
#include <stdlib.h>

long last = 0;

void got_value(int slot, long value, unsigned short op) {
  if (!running || value >= COUNT_TO) {
    running = 0;
    // wait a bit for the nodes to stop screaming
    // and then shutdown
    sleep(2);
    stop_intheory();
    exit(0);
  }
  if (value <= last) return;
  last = value;
  set_it(SLOT, value + 1, ASYNC_SEND);
}

void loop() {
  while (running) { sleep(1); }
}

int main() {
  set_log_level(TRACE);
  char * all_nodes[] = {"A", "B", "C", "D"};
  return spawn_nodes(all_nodes);
}


