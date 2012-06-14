#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "include/intheory.h"
#include "include/network.h"
#include "include/proposer.h"

void discard(void *thing) {
  free(thing);
}

void intheory_sm(enum role_t role) {
  state s;
  s.type = s.node_num = s.nodes_left = s.slot = s.value = -1;
  s.ticket = 0;
  s.depth = 0; 
  switch(role) {
  case PROPOSER:
    s.state = S_AVAILABLE;
    sm_proposer(s);
    break;
  case ACCEPTOR:
    break;
  case LEARNER:
    break;  
  case CLIENT:
    break;
  }
}

