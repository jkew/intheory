#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>
#include "include/intheory.h"
#include "include/state_machine.h"
#include "include/network.h"
#include "include/acceptor.h"
#include "include/logger.h"
#include "include/util.h"

state sm_acceptor_available(state s) {
  assert(s.nodes_left == -1 && s.ticket == -1 
	 && s.type == -1 && s.slot == -1);
  message *mesg = recv_from(ACCEPTOR,-1, -1, PROPOSAL);
  if (mesg == 0) { return s; }

  // If we are locking; and FAIL_EARLY is
  // set, do not proceed if the value is
  // already set.
  unsigned short f = LOCK | FAIL_EARLY;
  if ((mesg->flags & f) == f) {
    if (exists(mesg->slot) == TRUE) {
      send_to(mesg->from, mesg->ticket, REJECTED_PROPOSAL, mesg->slot, mesg->value, mesg->flags);
      discard(mesg);
      return s;
    }
  }

  s.slot = mesg->slot;
  s.value = mesg->value;
  s.state = S_ACCEPT_PROPOSAL; // We have not accepted any proposal yet
  s.ticket = mesg->ticket;
  s.type = PROPOSAL;
  s.flags = mesg->flags;
  s.client = mesg->from;
  s.deadline = get_deadline(deadline);
  discard(mesg);
  return s;
}

state sm_acceptor_accept(state s) {
  assert(s.nodes_left == -1 && s.ticket >= 0 && s.type == PROPOSAL);

  // send acceptance
  s.type = ACCEPTED_PROPOSAL;
  s.state = S_ACCEPTED_WAIT;
  send_to(s.client, s.ticket, s.type, s.slot, s.value, s.flags);
  return s;
}

state sm_acceptor_accepted(state s) {
  assert(s.nodes_left == -1 && s.ticket >= 0 && s.type == ACCEPTED_PROPOSAL);
  message *mesg = recv_from(ACCEPTOR,-1, s.slot, PROPOSAL | ACCEPTOR_SET);
  if (mesg == 0) {
    if (!deadline_passed(s.deadline)) {
      trace("Acceptor still waiting... ");
      return s;
    }
    // unable to receive message from proposer, or any other
    // system
    error("ACCEPTOR: No response from proposer");
    s.state = S_DONE;
    s.type = s.client = s.nodes_left = s.slot = s.ticket = s.value = -1;
    return s;
  }
  // New proposal received after acceptance
  if (mesg->type == PROPOSAL) {
    // ticket greater, accept again
    if (mesg->ticket > s.ticket) {
      trace("ACCEPTOR: Recieved an updated ticket from some proposer, accepting that one!");
      s.ticket = mesg->ticket;
      s.value = mesg->value;
      s.type = PROPOSAL;
      s.client = mesg->from;
      s.state = S_ACCEPT_PROPOSAL;
      discard(mesg);
      return s;
    } else {
      // proposal not worth considering
      send_to(mesg->from, s.ticket, REJECTED_PROPOSAL, s.slot, s.value, s.flags);
      // no state change
      discard(mesg);
      return s;
    }
  }

  if (mesg->type == ACCEPTOR_SET) {
    if (mesg->ticket != s.ticket || s.client != mesg->from) { // was !=
      // this is an old ticket, ignore
      // or this is from a proposer we have relinquished our promise to
      // should we send a reject message here?
      // ?   send_to(mesg->from, s.ticket, REJECTED_PROPOSAL, s.slot, s.value);
      discard(mesg);
      return s;
    }
    s.state = S_SET;
    s.nodes_left = num_nodes();
    s.type = SET;
    discard(mesg);
    return s;
  }
}

state sm_acceptor_set(state s) {
  assert(s.nodes_left > 0 && s.ticket >= 0 && s.type == SET);
  send_to((s.nodes_left - 1) % (num_nodes()), s.ticket, SET, s.slot, s.value, s.slot);
  s.nodes_left--;
  if (s.nodes_left <= 0) {
    s.state = S_DONE;
    return s;
  }
    
  return s;
}

state sm_acceptor(state s) {
  if (s.state != S_AVAILABLE)
    log_state(s, ACCEPTOR);
  state latest_state = s;
  switch(s.state) {
  case S_AVAILABLE:
    latest_state = sm_acceptor_available(s);
    break;
  case S_ACCEPT_PROPOSAL:
    latest_state = sm_acceptor_accept(s);
    break;
  case S_ACCEPTED_WAIT:
    latest_state = sm_acceptor_accepted(s);
    break;
  case S_SET:
    latest_state = sm_acceptor_set(s);
    break;
  default:
    assert(0);
    break;
  }

  return latest_state;
}

