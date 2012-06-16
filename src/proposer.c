#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "include/intheory.h"
#include "include/state_machine.h"
#include "include/network.h"
#include "include/proposer.h"
#include "include/logger.h"

state sm_proposer_available(state s) {
  state latest_state = s;
  assert(s.nodes_left = -1 && s.node_num == -1 && s.ticket >= 0 
	 && s.type == -1 && s.slot == -1);
  message* mesg = recv_from(PROPOSER,-1, -1, CLIENT_VALUE);
  s.type = CLIENT_VALUE;
  s.slot = mesg->slot;
  s.value = mesg->value;
  s.state = S_PREPARE;
  latest_state = sm_proposer(s);
  if (latest_state.state == S_ACCEPTED_PROPOSAL && latest_state.value == s.value) {
    send_to(mesg->from, latest_state.ticket, WRITE_SUCCESS, s.slot, latest_state.value);
  } else {
    send_to(mesg->from, latest_state.ticket, WRITE_FAILED, s.slot, latest_state.value);
  }
  discard(mesg);
  return latest_state;
}

state sm_proposer_prepare(state s) {
  assert(s.nodes_left = -1 && s.node_num == -1 && s.ticket >= 0 
	 && s.type == CLIENT_VALUE && s.slot >= 0);
  // send to a quorom of acceptors
  int quorom_size = ((int) num_nodes / 2) + 1;
  // send a few more proposals up-to the number required for acceptor faults
  int failsafe_acceptors = ((int)quorom_size / 3);
  int start_node = random() % num_nodes;
  s.type = PROPOSAL;
  s.state = S_SEND_PROPOSAL_TO_ACCEPTOR;
  s.ticket = s.ticket + 1;
  s.node_num = start_node;
  s.nodes_left = quorom_size + failsafe_acceptors;
  return sm_proposer(s);
}

state sm_proposer_send_proposal(state s) {
  assert(s.nodes_left > 0 && s.node_num >= 0 && s.ticket >= 0 
	 && s.type == PROPOSAL && s.slot >= 0);
  int to_node = s.node_num % num_nodes;
  while (!send_to(to_node, s.ticket, s.type, s.slot, s.value)) {
    to_node = (to_node++) % num_nodes;
  }
  s.state = S_COLLECT_ACCEPTOR_PROPOSAL_RESPONSE;
  s.node_num = to_node;
  return sm_proposer(s);
}

state sm_proposer_collect(state s) {
   assert(s.nodes_left > 0 && s.node_num >= 0 && s.ticket >= 0 
	  && s.type == PROPOSAL && s.slot >= 0);
   // really want to filter by ACCEPTED_PROPOSAL or REJECTED HERE
   message* response = recv_from(PROPOSER,s.node_num, s.slot, ACCEPTED_PROPOSAL | REJECTED_PROPOSAL); 
   if (response == NULL) { // failed to receive, try again with a new node
     error("! Failed to recieve message from acceptor");     
     s.state = S_SEND_PROPOSAL_TO_ACCEPTOR;
     s.node_num++;
     return sm_proposer(s);
   }
   assert(response->slot == s.slot);
   if (response->type == REJECTED_PROPOSAL) { // rejected directly, propose with a new ticket
     info("Acceptor rejected proposal");
     // delay?
     s.state = S_PREPARE;
     s.type = CLIENT_VALUE;
     s.node_num = s.nodes_left = -1;
     discard(response); 
     return sm_proposer(s);
   }
   assert(response->type == ACCEPTED_PROPOSAL);
   if (response->ticket > s.ticket) {
     //TODO: Will we ever hit this branch with the current acceptor code?
     info("Recieved updated ticket from acceptor");	  
     s.value = response->value;
     s.ticket = response->ticket;
   }
   if (s.nodes_left > 1) {
     // continue to contact acceptors
     state next_acceptor = s;
     next_acceptor.state = S_SEND_PROPOSAL_TO_ACCEPTOR;
     next_acceptor.node_num++;
     next_acceptor.nodes_left--;
     state return_state = sm_proposer(next_acceptor);
     s.ticket = return_state.ticket;
     s.value = return_state.value;
     s.state = S_ACCEPTED_PROPOSAL;
     s.nodes_left = -1;
     discard(response);
     return sm_proposer(s);
   } else {
     // yay. all required acceptors accepted     
     s.state = S_ACCEPTED_PROPOSAL;
     s.nodes_left = -1;
     discard(response);     
     return sm_proposer(s);
   }
}

state sm_proposer(state s) {
  log_state(s, PROPOSER);
  state latest_state = s;
  s.depth++;
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
    ;
    // send value to all nodes
    send_to(s.node_num % num_nodes, s.ticket, ACCEPTOR_SET, s.slot, s.value);
    break;
  default:
    assert(0);
    break;
  }
  log_state(latest_state, PROPOSER);
  s.depth--;
  return latest_state;
}

