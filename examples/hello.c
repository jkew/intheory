#include <../src/include/intheory.h>
#include <stdio.h>
#include <stdlib.h>

#define SLOT 0

int hellos_received = 0;


void say_hello() {
  if (!set_it(SLOT, my_id())) {
    printf("ERROR: Can't get a word in!");
    exit(1);
  }
}

void got_hello(long slot, long value) {
  if (value >= 0) {
    hellos_received++;
    printf("Received my %d th Hello! from node %d\n", hellos_received, value);
  }
}

int main(int argc, char **args) {
  if (argc < 5) {
    printf("Usage: %s LOCAL_ADDRESS:LOCAL_PORT second_node:port third_node:port fourth_node:port ...\n", args[0]);
    printf("   ex: %s 10.0.0.1:4321 10.0.0.2:4321 10.0.0.3:4321 10.0.0.4:4321 ...\n", args[0]);
    return 1;
  }

  set_log_level(GRAPH);
  char * all_nodes[argc - 1];

  all_nodes[0] = args[1];
  int i = 1;
  for (; i < argc; i++) {
    all_nodes[i] = args[i + 1];
  }


  start_intheory(0, argc - 1, all_nodes);

  register_changed_cb(SLOT, got_hello);
  printf("MY ID: %d\n", my_id());
  if (my_id() == 0) {
    printf("I guess I'm the designated hello-er! Start your nodes!\n");
    sleep(5);
    say_hello();
  }

  while (hellos_received < 1) { sleep(1); }

  stop_intheory();
  return 0;
}


