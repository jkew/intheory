#include "../src/include/intheory.h"
#include "../src/include/state_machine.h"
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

void writer( void *arg) {
  printf("Writer started\n");
  int rounds = 100;
  while(rounds--) {
     message *msg = create_message(1, 2, 3, CLIENT_VALUE, 4, 999);
     add_message(msg);
  }
}

void test_ring() {
   const char * test_nodes[] = { "memyselfi:5229", 
				 "zebra:321", 
				 "apple:123",
				 "intheory:876"};
   init_network(4, test_nodes, 10);
   // spot check that the nodes list was populated correctly
   assert(my_id() == 2);
   assert(get_port(3) == 321);

   // verify empty
   int i = 10;
   while(i--)
     assert(get_if_matches(i, -1, -1, 0xFFFFFFFFF) == 0);

   // add messages, verify some pattern matching
   message *msg = create_message(1, 2, 3, CLIENT_VALUE, 4, 999);
   add_message(msg);
   message *result = get_if_matches(0, 1, 4, SET);
   assert(result == 0);
   result = get_if_matches(0, 1, 3, CLIENT_VALUE);
   assert(result == 0);
   result = get_if_matches(0, 2, 4, CLIENT_VALUE);
   assert(result == 0);
   result = get_if_matches(0, 1, 4, CLIENT_VALUE);
   assert(result != 0);
   assert(result->value == 999);
   free(result);

   msg = create_message(1, 2, 3, CLIENT_VALUE, 4, 999);
   add_message(msg);
   result = get_if_matches(1, 1, 4, CLIENT_VALUE | SET);
   assert(result != 0);
   assert(result->value == 999);
   free(result);

   // verify empty
   i = 10;
   while(i--)
     assert(get_if_matches(i, -1, -1, 0xFFFFFFFFF) == 0);

   pthread_t writer_thread;
   pthread_create(&writer_thread, NULL, writer, 0);
   printf("Reading...\n");
   int rounds = 100;
   while(rounds--) {
     msg = recv_from(PROPOSER, 1, 4, CLIENT_VALUE);
     assert(msg != 0);
   }
   pthread_join(writer_thread, 0);
   destroy_network();
}

void test_crc() {
  message *msg = create_message(1, 2, 3, CLIENT_VALUE, 4, 999);
  assert(crc_valid(msg));
}

void test_network() {
  test_crc();
  test_ring();
}
