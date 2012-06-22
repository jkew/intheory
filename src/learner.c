#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include "include/intheory.h"
#include "include/state_machine.h"
#include "include/network.h"
#include "include/learner.h"
#include "include/logger.h"

#define INITIAL_SLOT_SIZE 256

typedef struct {
  long slot;
  long value;
  int runstate;
  slot_changed_cb cb;
  pthread_t thread;
} callback;

long *slots = 0;
int maxslot = 0;
callback *cbs = 0;
int maxcbs = 0;

void init_learner() {
  slots = malloc(sizeof(long)*INITIAL_SLOT_SIZE);
  maxslot = 255;
}

void destroy_cb() {
  int i;
  for(i = 0; i < maxcbs; i++) {
    if (cbs[i].runstate == 1) {
      cbs[i].runstate = 0;
      pthread_join(&cbs[i].thread, 0);
    }
  }
  discard(cbs);
}

void destroy_learner() {
  discard(slots);
  slots = 0;
  maxslot = 0;
  destroy_cb();
}

void cb_worker(void *callback_struct) {
  callback *cbs = callback_struct; 
  cbs->cb(cbs->slot, cbs->value);
  cbs->runstate = 1; 
}

void slot_changed(long slot, long value) {
  int i;
  for(i = 0; i < maxcbs; i++) {
    if (cbs[i].slot == slot) {
      cbs[i].value = value;
      pthread_create(&(cbs[i].thread), 0, cb_worker, &cbs[i]);
      cbs[i].runstate = 2;
    } else {
      if (cbs[i].runstate == 1) {
	cbs[i].runstate = 0;
	pthread_join(&cbs[i].thread, 0);
      }
    }
  }
}

void register_changed_cb(long slot, slot_changed_cb cb) {
  int newsize = maxcbs + 1;
  callback *tmp = malloc(sizeof(callback)*newsize);
  memcpy(tmp, cbs, maxcbs);
  callback *old = cbs;
  cbs = tmp;
  discard(old);
  cbs[maxcbs].slot = slot;
  cbs[maxcbs].cb = cb;
  cbs[maxcbs].runstate = 0;
  cbs[maxcbs].thread = 0;
  maxcbs = newsize;
}

void unregister_cb(long slot, slot_changed_cb cb) {
  int i;
  for(i = 0; i < maxcbs; i++) {
    if (cbs[i].slot == slot && cbs[i].cb == cb) {
      cbs[i].slot = -1;
      return;
    }
  }
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
  long oldval = slots[slot];
  slots[slot] = value;
  slot_changed(slot, value);
}

long get(int slot) {
  return slots[slot];
}

state sm_learner_available(state s) {
  assert(s.nodes_left == -1 && s.type == -1);
  message *mesg = recv_from(LEARNER, -1, -1, SET | GET);
  if (mesg == 0) { return s; }
  s.depth++;
  s.slot = mesg->slot;
  s.client = mesg->from;
  if (mesg->type == GET) {
    s.type = GET;
    s.state = S_GET;
  } else {
    if (mesg->ticket <= s.ticket) {
      discard(mesg);
      return s;
    }
    s.ticket = mesg->ticket;
    set(mesg->slot, mesg->value);
    s.state = S_DONE;
  }
  discard(mesg);
  return s;
}

state sm_learner_get(state s) {
  assert(s.type == GET && s.slot >= 0 && s.client >= 0);
  if (s.slot <= maxslot) {
    send_to(s.client, -1, READ_SUCCESS, s.slot, get(s.slot));     
  } else {
    send_to(s.client, -1, READ_FAILED, s.slot, -1);     
  }
  s.state = S_DONE;
  s.type = s.client = s.nodes_left = s.slot = s.ticket = s.value = -1;
  return s;
}

state sm_learner(state s) {
  // TODO: move to normal init spot
  if (slots == 0) {
    init_learner();
  }
  if (s.state != S_AVAILABLE)
    log_state(s, LEARNER);
  state latest_state = s;
  switch(s.state) {
  case S_AVAILABLE:
    latest_state = sm_learner_available(s);
    break;
  case S_SET:
    assert(0);
    break;
  case S_GET:
    latest_state = sm_learner_get(s);
    break;
  default:
    assert(0);
    break;
  }
  return latest_state;
}

