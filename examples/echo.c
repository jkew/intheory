#include <../src/include/intheory.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **args) {
  if (argc < 5) {
    printf("Usage: %s LOCAL_PORT second_node:port third_node:port fourth_node:port ...\n", args[0]);
    printf("   ex: %s 4321 10.0.0.1:4321 10.0.0.2:4321 10.0.0.3:4321 ...\n", args[0]);
    return 1;
  }
  char * me = "127.0.0.1:4321";
  char * other_nodes[argc - 2];

  //me = args[1];
  int i = 2;
  for (; i < argc; i++) {
    other_nodes[i - 2] = args[i];
  }
  start_intheory(me, argc - 2, other_nodes);
  while (1) { sleep(1); }
  stop_intheory();
  return 0;
}


