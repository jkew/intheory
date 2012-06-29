#include "../src/include/intheory.h"
#include "../src/include/state_machine.h"
#include "../src/include/network.h"
#include "include/test_common.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void test_network_nodes();
char * to_addr(char *nodePort);
int to_port(char *nodePort);

int main(int argc, char **args) {
  set_log_level(NONE);
  test_network_nodes();
  return 0;
}

void test_address_parse() {
   char * a = to_addr("127.0.0.1:1234");
   assert(strcmp(a, "127.0.0.1") == 0);
   int port = to_port("80.0.0.1:4321");
   assert(port == 4321);
 }

 void test_nodes() {
   const char * test_nodes[] = { "memyselfi:129", 
				 "zebra:321", 
				 "apple:123",
				 "intheory:876"};
   init_network_nodes(4, test_nodes);
   assert(my_id() == 2);
   assert(get_port(0) == 123);
   assert(get_port(1) == 876);
   assert(get_port(2) == 129);
   assert(get_port(3) == 321);
   char *got = get_address(0);
   assert(strcmp(got, "apple") == 0);
   free(got);
   got = get_address(1);
   assert(strcmp(got, "intheory") == 0);
   free(got);
   got = get_address(2);
   assert(strcmp(got, "memyselfi") == 0);
   free(got);
   got = get_address(3);
   assert(strcmp(got, "zebra") == 0);
   free(got);
   destroy_network_nodes();
}

void test_network_nodes() {
  test_address_parse();
  test_nodes();
}
