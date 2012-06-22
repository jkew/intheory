#include <../src/include/intheory.h>
#include <stdio.h>
#include <stdlib.h>

#define SLOT 0
#define RETURN_HELLOS 10

int hellos_left = RETURN_HELLOS;
int hellos_received = 0;
int sending = 0;


void say_hello() {
  printf("Sending my hello! Hellos Left %d\n", hellos_left);
  if (!set_it(SLOT, my_id())) {
    printf("ERROR: Can't get a word in!");
    exit(1);
  }
  sending = 0;
  hellos_left--;
}

void got_hello(long slot, long value) {
  if (value != my_id()) {

    if (value < 0) {
      hellos_left = 0;
    }

    printf("Received Hello! from node %d\n", value);
    hellos_received++;
    if (hellos_left && !sending) {
      sending++;
      say_hello();
    } 
  }
}

int main(int argc, char **args) {
  if (argc < 5) {
    printf("Usage: %s LOCAL_ADDRESS:LOCAL_PORT second_node:port third_node:port fourth_node:port ...\n", args[0]);
    printf("   ex: %s 10.0.0.1:4321 10.0.0.2:4321 10.0.0.3:4321 10.0.0.4:4321 ...\n", args[0]);
    return 1;
  }

  set_log_level(NONE);
  char * me;
  char * other_nodes[argc - 2];

  me = args[1];
  int i = 2;
  for (; i < argc; i++) {
    other_nodes[i - 2] = args[i];
  }


  start_intheory(me, argc - 2, other_nodes);
  register_changed_cb(SLOT, got_hello);
  printf("MY ID: %d\n", my_id());
  if (my_id() == 0) {
    printf("I guess I'm the first to say hello!\n");
    sleep(5);
    say_hello();
  }

  while (hellos_left) { sleep(1); }

  printf("This is a stupid conversation. I'm ending it.");
  set_it(SLOT, -1);
  stop_intheory();
  return 0;
}


