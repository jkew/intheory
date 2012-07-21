#include <include/state_machine.h>
#include <include/proposer.h>
#include <include/acceptor.h>
#include <include/learner.h>
// TODO: Ideally we should have one state per slot
state saved_proposer;
state saved_acceptor;
state saved_learner;

void init_sm() {
  saved_learner = init_state(LEARNER, 0);
  saved_proposer = init_state(PROPOSER, 0);
  saved_acceptor = init_state(ACCEPTOR, 0);
}

void next_states() {
  int p_state, a_state, l_state;
  
  do {
    p_state = saved_proposer.state;
    a_state = saved_acceptor.state;
    l_state = saved_learner.state;
    next_state(PROPOSER);
    next_state(ACCEPTOR);
    next_state(LEARNER);
    usleep(10000);
  } while (saved_proposer.state != p_state
	   || saved_acceptor.state != a_state
	   || saved_learner.state != l_state);
    
}

state init_state(enum role_t role, state *prev_state) {
  state s;
  s.type = s.num_quorom = s.max_fails = s.client = s.nodes_left = s.slot = s.value = s.ticket =-1;
  s.depth = s.fails = 0; 
  s.state = S_AVAILABLE;
  switch(role) {
  case LEARNER:
    if (prev_state != 0) {
      s.slot = prev_state->slot;
      s.value = prev_state->value;
    }
  case PROPOSER:
    if (prev_state != 0) 
      s.ticket = prev_state->ticket;
    else
      s.ticket = 0;
  case ACCEPTOR:
  case CLIENT:
    break;
  }
  return s;
}

void next_state(enum role_t role) {
  state *s;
  sm_role_fn sm = 0;
  switch(role) {
  case PROPOSER:
    s = &saved_proposer;
    sm = sm_proposer;
    break;
  case ACCEPTOR:
    s = &saved_acceptor;
    sm = sm_acceptor;
    break;
  case LEARNER:
    s = &saved_learner;
    sm = sm_learner;
    break;
  case CLIENT:
    assert(0);
    break;
  }
  state new_state = sm(*s);

  if (new_state.state == S_DONE) {
    new_state = init_state(role, &new_state);
  }
  *s = new_state;
  return;
}

