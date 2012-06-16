#include "../src/include/intheory.h"
#include "../src/include/network.h"
#include "include/test_common.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void test_network();
message * get_if_matches(int, int,long, unsigned int);

int main(int argc, char **args) {
  test_network();
  return 0;
}

void test_server() {
  printf("test_server\n");
   const char * test_nodes[] = { "127.0.0.1:4321", 
				 "zebra:321", 
				 "apple:123",
				 "intheory:876"};
   message *msg, *result;
   init_network(4, test_nodes, 10);
   start_server();
   msg = create_message(1, 2, 3, CLIENT_VALUE, 4, 999);   
   send_intheory(my_id(), msg);
   send_intheory(my_id(), msg);
   sleep(1);
   result = get_if_matches(0, 1, 4, CLIENT_VALUE);
   assert(result != 0);
   assert(result->value == 999);
   result = get_if_matches(1, 1, 4, CLIENT_VALUE);
   assert(result != 0);
   free(result);
   free(msg);
   stop_server();
   destroy_network();
}

void test_network() {
  test_server();
}
