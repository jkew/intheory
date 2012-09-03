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
  list_ref *active_list = new_list();
  listi itr = list_itr(states);
  assert(sizel(states) >= 3);
  while(there(itr)) {
    slot_state_t *curr = (slot_state_t *)value_itr(itr);
    assert(curr != NULL);
    int delete_state = 0;
    if (curr->state == S_AVAILABLE) {
      // Only ten available state per role
      if (avail[curr->role] > 10) {
	delete_state = 1;
      } else {
	avail[curr->role]++;
	curr->slot = -1;
	curr->pstate = -1;
      }
    } else {      
      // sm operating on some state
      // if the slot/role has been seen previously, skip
      // TODO: Really this should iterate over a hashmap
      //       and only run the first state to completion
      //       of each list, but I'm going to do the slow
      //       and correct thing first.
      listi aitr = list_itr(active_list);

      while(there(aitr)) {
	slot_state_t *active_state = (slot_state_t *)value_itr(aitr);
	assert(active_state != NULL);	
	if (active_state != curr && active_state->role == curr->role && active_state->slot == curr->slot) {
	  error("role %d has the same state %d as another, killing\n", curr->role, curr->state);
	  delete_state = 1;
	}
	aitr = next_itr(aitr);
      }
      if (!delete_state) pushv(active_list, curr);
    }

    if (delete_state) {
      slot_state_t *delete = NULL;
      itr = remove_itr(states, itr, (void **) &delete);	
      assert(delete == curr);
      fsck(states);
      discard(delete);
      continue;
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
      error("CREATING EXTRA STATE FOR ROLE %d", i);
      pushv(states, new_slot_state(i));
    }
  }
  slot_state_t *delete = NULL;
  while(delete = popv(active_list));
  free(active_list);
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

