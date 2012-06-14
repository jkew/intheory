#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "include/intheory.h"
#include "include/network.h"
#include "include/proposer.h"
#include "include/acceptor.h"
#include "include/learner.h"

void discard(void *thing) {
  free(thing);
}

void intheory_sm(enum role_t role) {
  state s;
  s.type = s.node_num = s.nodes_left = s.slot = s.value = -1;
  s.depth = 0; 
  s.state = S_AVAILABLE;
  s.ticket = -1;
  switch(role) {
  case PROPOSER:
    s.ticket = 0;
    sm_proposer(s);
    break;
  case ACCEPTOR:
    sm_acceptor(s);
    break;
  case LEARNER:
    sm_learner(s);
    break;  
  case CLIENT:
    break;
  }
}

