#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>
#include "include/intheory.h"
#include "include/state_machine.h"
#include "include/network.h"
#include "include/proposer.h"
#include "include/logger.h"
#include "include/util.h"

state sm_proposer_available(state s) {
  state latest_state = s;
  assert(s.nodes_left = -1 && s.ticket >= 0 
	 && s.type == -1 && s.slot == -1);
  message* mesg = recv_from(PROPOSER,-1, -1, CLIENT_VALUE);
  if (mesg == 0) { return s; }
  s.type = CLIENT_VALUE;
  s.client = mesg->from;
  s.slot = mesg->slot;
  s.value = mesg->value;
  s.state = S_PREPARE;
  set_deadline(deadline, &(s.deadline));
  return s;
}

state sm_proposer_prepare(state s) {
  assert(s.nodes_left = -1 && s.ticket >= 0 
	 && s.type == CLIENT_VALUE && s.slot >= 0);
  // send to a quorom of acceptors
  int quorom_size = ((int) num_nodes / 2) + 1;
  // send a few more proposals up-to the number required for acceptor faults
  int failsafe_acceptors = ((int)quorom_size / 3);
  trace("Picking a quorom size of %d with %d failsafe acceptors from %d total nodes", quorom_size, failsafe_acceptors, num_nodes);
  int node = random() % num_nodes;
  s.type = PROPOSAL;
  s.state = S_SEND_PROPOSAL_TO_ACCEPTOR;
  s.ticket = s.ticket + 1;
  s.num_quorom = quorom_size;
  s.max_fails = failsafe_acceptors;
  s.nodes_left = s.num_quorom = quorom_size + failsafe_acceptors;
  s.fails = 0;
  assert(s.num_quorom <= MAX_QUOROM_SIZE);
  assert(s.num_quorom <= num_nodes);
  int i;
  for (i = 0; i < s.num_quorom; i++) {
    s.nodes_quorom[i] = node;
    node = (node + 1) % num_nodes;
  }
  return s;
}

state sm_proposer_send_proposal(state s) {
  assert(s.nodes_left >= 0 && s.ticket >= 0 
	 && s.type == PROPOSAL && s.slot >= 0);


  if (s.nodes_left <= 0) {
    // sent all proposals, move onto collection
    s.state = S_COLLECT_ACCEPTOR_PROPOSAL_RESPONSE;
    s.nodes_left = s.num_quorom;
    return s;
  }
 
  int to_node = s.nodes_quorom[s.nodes_left - 1];
  trace("Sending proposal to node %d", to_node);
  if (send_to(to_node, s.ticket, s.type, s.slot, s.value)) {
    // send succeeded, same state, but move on to a different acceptor node
    trace("Proposal was sent successfully to node %d", to_node);
    s.nodes_left--;
    return s;
  } else {
    // send failed, pick a new node
    error("Failed to send message to acceptor %d fails %d", to_node, s.fails);
    s.fails++;
    if (s.fails > s.max_fails) {
      error("Failed to send proposal to acceptors %d times, failing", s.fails);
      s.state = S_CLIENT_RESPOND;
      s.type = WRITE_FAILED;
      return s;
    }
    s.nodes_left--;
    s.num_quorom--;
    // acceptable loss, remove that node
    s.nodes_quorom[s.nodes_left] = s.nodes_quorom[s.num_quorom];
  }
  return s;
}

state sm_proposer_collect(state s) {
   assert(s.nodes_left > 0 && s.ticket >= 0 
	  && s.type == PROPOSAL && s.slot >= 0);
   int node = s.nodes_quorom[s.nodes_left - 1];
   message* response = recv_from(PROPOSER,node, s.slot, ACCEPTED_PROPOSAL | REJECTED_PROPOSAL); 
   if (response == NULL) { 
     if (!deadline_passed(&(s.deadline))) {
	 return s;
     } 

     // failed to receive any message
     error("! Failed to recieve message from acceptor %d fails %d",  node, s.fails);     
     s.fails++;
     if (s.fails > s.max_fails) {
      error("Failed to send proposal to acceptors %d times, failing", s.fails);
      s.state = S_CLIENT_RESPOND;
      s.type = WRITE_FAILED;
      return s;
     }
     s.nodes_left--;
     s.num_quorom--;
     // acceptable loss, remove that node
     s.nodes_quorom[s.nodes_left] = s.nodes_quorom[s.num_quorom];

     if (s.nodes_left <= 0) {
       // good enough
       s.state = S_ACCEPTED_PROPOSAL;
       s.nodes_left = s.num_quorom;
       discard(response);     
       return s;
     }

     return s;
   }

   // we actually received a message

   assert(response->slot == s.slot);

   // TODO: Review carefully
   if (response->type == REJECTED_PROPOSAL || response->ticket > s.ticket) { 
     // rejected directly, propose with a new ticket
     info("Acceptor rejected proposal, retrying ");
     // delay?
     int usec_delay = random() % 1000000;
     usleep(usec_delay);
     s.state = S_PREPARE;
     s.type = CLIENT_VALUE;
     s.fails = 0;
     s.nodes_left = -1;
     if (response->ticket > s.ticket) {
       s.value = response->value;
       s.ticket = response->ticket;
     }
     discard(response); 
     return s;
   }

   assert(response->type == ACCEPTED_PROPOSAL);
   //if (response->ticket > s.ticket) {
   //  error("Unexpectedly received an updated ticket from an acceptor as part of an ACCEPTED_PROPOSAL");
   //  s.state = S_CLIENT_RESPOND;
   //  s.type = WRITE_FAILED;
   //  return s;
   //}

   if (s.nodes_left > 1) {
     // continue to wait for responses
     s.nodes_left--;
     discard(response);
     return s;
   } else {
     // yay. all required acceptors accepted, move onto client accept 
     s.state = S_ACCEPTED_PROPOSAL;
     s.nodes_left = s.num_quorom;
     discard(response);     
     return s;
   }
}

state sm_proposer_accepted(state s) {
  assert(s.nodes_left <= s.num_quorom);
  
  if (s.nodes_left <= 0) {
    s.state = S_CLIENT_RESPOND;
    s.type = WRITE_SUCCESS;
    return s;
  }

  // send value to node
  int node = s.nodes_quorom[s.nodes_left - 1];
  if (send_to(node, s.ticket, ACCEPTOR_SET, s.slot, s.value)) {
    // send succeeded
    s.nodes_left--;
    return s;
  } else {
    // send failed, but there's not much we can, or should do about it
    // the proposal was already accepted and we are proceeding to set the
    // value
    error("Unable to set value for acceptor %d, continueing anyways... ", node);
    s.fails++;
    s.nodes_left--;
    return s;
  }
}

state sm_proposer_respond_to_client(state s) {
  assert(s.type == WRITE_SUCCESS || s.type == WRITE_FAILED);
  send_to(s.client, s.ticket, s.type, s.slot, s.value);
  s.state = S_DONE;
  return s;
}

state sm_proposer(state s) {
  if (s.state != S_AVAILABLE)
    log_state(s, PROPOSER);
  state latest_state = s;
  switch(s.state) {
  case S_AVAILABLE:
    latest_state = sm_proposer_available(s);
    break;
  case S_PREPARE:
    latest_state = sm_proposer_prepare(s);
    break;
  case S_SEND_PROPOSAL_TO_ACCEPTOR:
    latest_state = sm_proposer_send_proposal(s);
    break;
  case S_COLLECT_ACCEPTOR_PROPOSAL_RESPONSE:
    latest_state = sm_proposer_collect(s);
    break;
  case S_ACCEPTED_PROPOSAL:
    latest_state = sm_proposer_accepted(s);
    break;
  case S_CLIENT_RESPOND:
    latest_state = sm_proposer_respond_to_client(s);
    break;
  default:
    assert(0);
    break;
  }

  return latest_state;
}

