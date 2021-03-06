#include "../src/include/intheory.h"
#include "../src/include/state_machine.h"
#include "../src/include/network.h"
#include "include/test_common.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void test_integration();

int main(int argc, char **args) {
  set_log_level(NONE);
  test_integration();
  return 0;
}

void test_integration() {
   const char * all_nodes[] = { "127.0.0.1:4321", 
				"127.0.0.1:4322", 
				"127.0.0.1:4323", 
				"127.0.0.1:4324", 
   };
   
   message *msg, *result;
   
   start_intheory(0 , 4, all_nodes);
   msg = create_message(1, 2, 3, CLIENT_VALUE, 4, 999, 0);
   send_intheory(my_id(), msg);
   sleep(5);
   stop_intheory();
}


