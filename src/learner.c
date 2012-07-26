#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include "include/intheory.h"
#include "include/state_machine.h"
#include "include/network.h"
#include "include/learner.h"
#include "include/store.h"
#include "include/logger.h"
#include "include/callbacks.h"

state sm_learner_available(state s) {
  assert(s.nodes_left == -1 && s.type == -1);
  message *mesg = recv_from(LEARNER, -1, -1, SET | GET);
  if (mesg == 0) { return s; }
  s.slot = mesg->slot;
  s.client = mesg->from;
  if (mesg->type == GET) {
    s.type = GET;
    s.state = S_GET;
  } else {
    if (mesg->ticket == s.ticket && mesg->slot == s.slot && mesg->value == s.value) {
      discard(mesg);
      return s;
    }
    s.ticket = mesg->ticket;
    s.value = mesg->value;
    s.flags = mesg->flags;
    set(mesg->slot, mesg->value, mesg->slot >= 0 ? -1 : get_deadline(deadline));
    s.state = S_DONE;
  }
  discard(mesg);
  return s;
}

state sm_learner_get(state s) {
  assert(s.type == GET && s.slot >= 0);
  if (exists(s.slot) == TRUE) {
    send_to(s.client, -1, READ_SUCCESS, s.slot, get(s.slot), s.flags);
  } else {
    send_to(s.client, -1, READ_FAILED, s.slot, -1, s.flags);
  }
  s.state = S_DONE;
  s.type = s.client = s.nodes_left = s.slot = s.ticket = s.value = -1;
  return s;
}

state sm_learner(state s) {
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

