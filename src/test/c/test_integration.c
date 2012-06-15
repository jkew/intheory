#include "../../main/c/include/intheory.h"
#include "../../main/c/include/network.h"
#include "include/test_common.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void test_integration();

int main(int argc, char **args) {
  test_integration();
  return 0;
}

void test_integration() {
  printf("test_server\n");
   const char * other_nodes[] = { "127.0.0.1:4322", 
				  "127.0.0.1:4323", 
				  "127.0.0.1:4324", 
   };
   
   message *msg, *result;
   

   start_intheory("127.0.0.1:4321", 3, other_nodes);
   msg = create_message(1, 2, 3, CLIENT_VALUE, 4, 999);   
   send_intheory(my_id(), msg);

   sleep(5);

   free(msg);
   stop_intheory();
}


