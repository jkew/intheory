#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "include/intheory.h"
#include "include/network.h"
#include "include/learner.h"
#include "include/logger.h"

#define INITIAL_SLOT_SIZE 256

long *slots = 0; 
int maxslot = 0;

void init_learner() {
  slots = malloc(sizeof(long)*INITIAL_SLOT_SIZE);
  maxslot = 255;
}

void destroy_learner() {
  discard(slots);
  slots = 0;
  maxslot = 0;
}

void set(int slot, long value) {
  if (slot > maxslot) {
    int new_size = slot * 2;
    long *oldslots = slots;
    long *newslots = malloc(sizeof(long)*new_size);

    memcpy(newslots, oldslots, maxslot + 1);    
    slots = newslots;
    maxslot = new_size - 1;
    discard(oldslots);
  }
  slots[slot] = value;
}

long get(int slot) {
  return slots[slot];
}

state sm_learner_available(state s) {
  assert(s.nodes_left == -1 && s.node_num == -1 && s.ticket == -1 
	 && s.type == -1 && s.slot == -1);
  message *mesg = recv_from(LEARNER, -1, -1, SET | GET);
  s.depth++;
  s.slot = mesg->slot;
  s.node_num = mesg->from;
  if (mesg->type == GET) {
    s.type = GET;
    s.state = S_GET;
  } else {
    int quorom_size = ((int) num_nodes / 2) + 1;
    s.value = mesg->value;
    s.ticket = mesg->ticket;
    s.nodes_left = quorom_size - 1;
    s.type = SET;
    s.state = S_SET;
  }
  discard(mesg);
  return sm_learner(s);
}

state sm_learner_set(state s) {
  assert(s.nodes_left > 0 && s.ticket >= 0 
	 && s.type == SET && s.slot >= 0);
  message *mesg = recv_from(LEARNER, -1, s.slot, SET);
  if (mesg == 0) {
    // did not receive message in time, go back to available
    error("No response from acceptor");
    s.state = S_AVAILABLE;
    s.type = s.node_num = s.nodes_left = s.slot = s.ticket = s.value = -1;
    return sm_learner(s);
  }
  
  if (s.ticket > mesg->ticket) {
    //ignore
    discard(mesg);
    return sm_learner(s);
  }

  if (s.ticket < mesg->ticket) {
    int quorom_size = ((int) num_nodes / 2) + 1;
    s.value = mesg->value;
    s.ticket = mesg->ticket;
    s.nodes_left = quorom_size - 1;
    discard(mesg);
    return sm_learner(s);
  }

  assert(s.value == mesg->value);

  s.nodes_left--;
  discard(mesg);
  if (s.nodes_left <= 0) {
    // drain additional messages for this slot
    while ((mesg = recv_from(LEARNER, -1, s.slot, SET)) != 0) { discard(mesg); }
    set(s.slot, s.value);
    return s;
  }
  // continue to wait for messages
  return sm_learner(s);
}

state sm_learner_get(state s) {
  assert(s.type == GET && s.slot >= 0 && s.node_num >= 0);
  if (s.slot <= maxslot) {
    send_to(s.node_num, -1, READ_SUCCESS, s.slot, get(s.slot));     
  } else {
    send_to(s.node_num, -1, READ_FAILED, s.slot, -1);     
  }
  s.state = S_AVAILABLE;
  s.type = s.node_num = s.nodes_left = s.slot = s.ticket = s.value = -1;
  return s;
}

state sm_learner(state s) {
  if (slots == 0) {
    init_learner();
  }
  log_state(s, LEARNER);
  s.depth++;
  state latest_state = s;
  switch(s.state) {
  case S_AVAILABLE:
    latest_state = sm_learner_available(s);
    destroy_learner();
    break;
  case S_SET:
    latest_state = sm_learner_set(s);
    break;
  case S_GET:
    latest_state = sm_learner_get(s);
    break;
  default:
    assert(0);
    break;
  }
  log_state(latest_state, LEARNER);
  latest_state.depth--;
  return latest_state;
}

