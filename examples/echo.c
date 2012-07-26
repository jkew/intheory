#include <../src/include/intheory.h>
#include <stdio.h>
#include <stdlib.h>

#define SLOT 0
#define RETURN_HELLOS 10

int hellos_left = RETURN_HELLOS;
int hellos_received = 0;

void say_hello() {
  printf("Sending my hello! Hellos Left %d\n", hellos_left);
  while (!set_it(SLOT, my_id(), 0)) {
    printf("ERROR: Can't get a word in!");
  }
  hellos_left--;
}


/**
 * Callback handler for changed slots.
 */
void got_hello(long slot, long value) {
  if (value != my_id()) {
    if (value < 0) {
      hellos_left = -1;
    }
    printf("Received hello from node %ld\n", value);
    hellos_received++;
  } else {
    printf("Received my own hello!\n");
  }
}

int main(int argc, char **args) {
  if (argc < 5) {
    printf("Usage: %s LOCAL_ADDRESS:LOCAL_PORT second_node:port third_node:port fourth_node:port ...\n", args[0]);
    printf("   ex: %s 10.0.0.1:4321 10.0.0.2:4321 10.0.0.3:4321 10.0.0.4:4321 ...\n", args[0]);
    return 1;
  }

  set_log_level(ERROR);
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
    printf("Since I'm node 0 I'll kick this off with the first hello... waiting 5 seconds\n");
    sleep(5);
    say_hello();
  }

  int last_hello_count = 0;
  while (hellos_left > 0) {
    if (last_hello_count != hellos_received) {      
      last_hello_count = hellos_received;
      say_hello();
    } else {
      sleep(1);
    }
  }

  printf("This is a stupid conversation. I'm ending it.");
  set_it(SLOT, -1, 0);

  // wait to recieve our -1 before quiting
  while (hellos_left >= 0) {
    sleep(1);
  }

  stop_intheory();

  return 0;
}


