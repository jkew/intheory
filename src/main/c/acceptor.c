#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "include/intheory.h"
#include "include/network.h"
#include "include/acceptor.h"
#include "include/logger.h"


state sm_acceptor_available(state s) {
  assert(s.nodes_left == -1 && s.node_num == -1 && s.ticket == -1 
	 && s.type == -1 && s.slot == -1);
  message *mesg = recv_from(ACCEPTOR,-1, -1, PROPOSAL);
  s.depth++;
  s.slot = mesg->slot;
  s.value = mesg->value;
  s.state = S_ACCEPT_PROPOSAL; // We have not accepted any proposal yet
  s.ticket = mesg->ticket;
  s.type = PROPOSAL;
  s.node_num = mesg->from;
  discard(mesg);
  return sm_acceptor(s);
}

state sm_acceptor_accept(state s) {
  assert(s.nodes_left == -1 && s.node_num >= 0 && s.ticket >= 0 
	 && s.type == PROPOSAL && s.slot >= 0);
  // send acceptance
  s.type = ACCEPTED_PROPOSAL;
  s.state = S_ACCEPTED_WAIT;
  send_to(s.node_num, s.ticket, s.type, s.slot, s.value);
  return sm_acceptor(s);
}

state sm_acceptor_accepted(state s) {
  assert(s.nodes_left == -1 && s.node_num >= 0 && s.ticket >= 0 
	 && s.type == ACCEPTED_PROPOSAL && s.slot >= 0);
  message *mesg = recv_from(ACCEPTOR,-1, s.slot, PROPOSAL | ACCEPTOR_SET);
  if (mesg == 0) {
    // unable to receive message from proposer, or any other
    // system
    error("No response from proposer");
    s.state = S_AVAILABLE;
    s.type = s.node_num = s.nodes_left = s.slot = s.ticket = s.value = -1;
    return sm_acceptor(s);
  }
  // New proposal received after acceptance
  if (mesg->type == PROPOSAL) {
    // ticket greater, accept again
    if (mesg->ticket > s.ticket) {
      s.ticket = mesg->ticket;
      s.value = mesg->value;
      s.type = PROPOSAL;
      s.node_num = mesg->from;
      s.state = S_ACCEPT_PROPOSAL;
      discard(mesg);
      return sm_acceptor(s);
    } else {
      // proposal not worth considering
      send_to(mesg->from, s.ticket, REJECTED_PROPOSAL, s.slot, s.value);
      // no state change
      discard(mesg);
      return sm_acceptor(s);
    }
  }

  if (mesg->type == ACCEPTOR_SET) {
    if (mesg->ticket < s.ticket || s.node_num != mesg->from) {
      // this is an old ticket, ignore
      // or this is from a proposer we have relinquished our promise to
      // should we send a reject message here?
      discard(mesg);
      return sm_acceptor(s);
    }
    s.state = S_SET;
    s.nodes_left = num_nodes;
    s.node_num = 0;
    s.type = SET;
    discard(mesg);
    return sm_acceptor(s);
  }
}

state sm_acceptor_set(state s) {
  assert(s.nodes_left > 0 && s.node_num >= 0 && s.ticket >= 0 
	 && s.type == SET && s.slot >= 0);
  send_to(s.node_num % num_nodes, s.ticket, SET, s.slot, s.value);
  s.nodes_left--;
  s.node_num++;
  if (s.nodes_left <= 0) {
    return s;
  }
    
  return sm_acceptor(s);
}

state sm_acceptor(state s) {
  log_state(s, ACCEPTOR);
  s.depth++;
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
  log_state(latest_state, ACCEPTOR);
  latest_state.depth--;
  return latest_state;
}

