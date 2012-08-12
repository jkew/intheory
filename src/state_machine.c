#include <include/state_machine.h>
#include <include/proposer.h>
#include <include/acceptor.h>
#include <include/learner.h>
#include <include/list.h>
#include <assert.h>
// TODO: Ideally we should have one state per slot

typedef struct {
  int slot;
  enum role_t role;
  state s;
  int state;
  int pstate;
} slot_state_t;

list_ref *states;

slot_state_t *new_slot_state(enum role_t role) {
  slot_state_t *s = malloc(sizeof(slot_state_t));
  s->s = init_state(role, 0);
  s->slot = -1;
  s->role = role;
  s->state = s->s.state;
  s->pstate = -1;
  return s;
}

void init_sm() {
  states = new_list();
  pushv(states, new_slot_state(LEARNER));
  pushv(states, new_slot_state(PROPOSER));
  pushv(states, new_slot_state(ACCEPTOR));
}

void destroy_sm() {
  slot_state_t *delete = NULL;
  while(delete = popv(states)) { free(delete); }
}

state next_state(enum role_t role, state current);

void next_states() {
  int avail[3] = { 0, 0, 0 };
  listi itr = list_itr(states);
  while(there(itr)) {
    slot_state_t *curr = (slot_state_t *)value_itr(itr);
    assert(curr != NULL);
    if (curr->state == S_AVAILABLE) {
      // Only one available state per role
      if (avail[curr->role]) {
	assert(avail[curr->role] == 1);
	slot_state_t *delete = NULL;
	itr = remove_itr(states, itr, (void **) &delete);
	discard(delete);
	continue;
      } else {
	avail[curr->role]++;
	assert(avail[curr->role] == 1);
      }
    } else {
      // sm operating on some state
      // if the slot/role has been seen previously, skip
      // TODO: Really this should iterate over a hashmap
      //       and only run the first state to completion
      //       of each list, but I'm going to do the slow
      //       and correct thing first.
    }
    curr->pstate = curr->state;
    curr->slot = curr->s.slot;
    curr->s = next_state(curr->role, curr->s);
    curr->state = curr->s.state;
    itr = next_itr(itr); 
  }

  int i = 0;
  for (i = 0; i < 3; i++) {
    if (avail[i] == 0) {
      pushv(states, new_slot_state(i));
    }
  }
}

state init_state(enum role_t role, state *prev_state) {
  state s;
  s.type = s.num_quorom = s.max_fails = s.client = s.nodes_left = s.slot = s.value = s.ticket =-1;
  s.fails = 0; 
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

state next_state(enum role_t role, state current) {
  sm_role_fn sm = 0;
  switch(role) {
  case PROPOSER:
    sm = sm_proposer;
    break;
  case ACCEPTOR:
    sm = sm_acceptor;
    break;
  case LEARNER:
    sm = sm_learner;
    break;
  case CLIENT:
    assert(0);
    break;
  }
  state new_state = sm(current);

  if (new_state.state == S_DONE) {
    new_state = init_state(role, &new_state);
  }
  return new_state;
}

