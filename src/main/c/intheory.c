#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "include/intheory.h"
#include "include/network.h"

void discard(void *thing) {
  printf("   free %p\n", thing);
  free(thing);
}

state sm_leader(state);

state sm_leader_available(state s) {
  state latest_state = s;
  assert(s.nodes_left = -1 && s.node_num == -1 && s.ticket >= 0 
	 && s.type == -1 && s.slot == -1 && s.value == -1);
  message* mesg = recv_from(-1, -1, CLIENT_VALUE);
  s.depth++;
  s.type = CLIENT_VALUE;
  s.slot = mesg->slot;
  s.value = mesg->value;
  s.state = S_PREPARE;
  latest_state = sm_leader(s);
  discard(mesg);
  return latest_state;
}

state sm_leader_prepare(state s) {
  assert(s.nodes_left = -1 && s.node_num == -1 && s.ticket >= 0 
	 && s.type == CLIENT_VALUE && s.slot >= 0 && s.value >= 0);
  // send to a quorom of acceptors
  int num_acceptors = ((int) num_nodes / 2) + 1;
  int start_node = random() % num_nodes;
  s.depth++;
  s.type = PROPOSAL;
  s.state = S_SEND_PROPOSAL_TO_ACCEPTOR;
  s.ticket = s.ticket + 1;
  s.node_num = start_node;
  s.nodes_left = num_acceptors;
  return sm_leader(s);
}

state sm_leader_send_proposal(state s) {
  assert(s.nodes_left > 0 && s.node_num >= 0 && s.ticket >= 0 
	 && s.type == PROPOSAL && s.slot >= 0 && s.value >= 0);
  int to_node = s.node_num % num_nodes;
  while (!send_to(to_node, s.ticket, s.type, s.slot, s.value)) {
    to_node = (to_node++) % num_nodes;
  }
  s.depth++;
  s.state = S_COLLECT_ACCEPTOR_PROPOSAL_RESPONSE;
  s.node_num = to_node;
  return sm_leader(s);
}

state sm_leader_collect(state s) {
   assert(s.nodes_left > 0 && s.node_num >= 0 && s.ticket >= 0 
	  && s.type == PROPOSAL && s.slot >= 0 && s.value >= 0);
   // really want to filter by ACCEPTED_PROPOSAL or REJECTED HERE
   message* response = recv_from(s.node_num, s.slot, -1); 
   if (response == NULL) { // failed to receive, try again with a new node
     printf("! Failed to recieve message from acceptor\n");
     s.depth++;
     s.state = S_SEND_PROPOSAL_TO_ACCEPTOR;
     s.node_num++;
     return sm_leader(s);
   }
   assert(response->slot == s.slot);
   if (response->type == REJECTED_PROPOSAL) { // rejected directly, propose with a new ticket
     printf("! Rejected proposal message from acceptor\n");
     // delay?
     s.depth++;
     s.state = S_PREPARE;
     s.type = CLIENT_VALUE;
     s.node_num = s.nodes_left = -1; 
     return sm_leader(s);
   }
   assert(response->type == ACCEPTED_PROPOSAL);
   if (response->ticket > s.ticket) {
     printf("? Recieved updated ticket from acceptor\n");
     s.value = response->value;
     s.ticket = response->ticket;
   }
   if (s.nodes_left > 1) {
     // continue to contact acceptors
     printf("? nodes left to propose to\n");
     s.depth++;
     state next_acceptor = s;
     next_acceptor.state = S_SEND_PROPOSAL_TO_ACCEPTOR;
     next_acceptor.node_num++;
     next_acceptor.nodes_left--;
     state return_state = sm_leader(next_acceptor);
     s.ticket = return_state.ticket;
     s.value = return_state.value;
     s.state = S_ACCEPTED_PROPOSAL;
     s.nodes_left = -1;
     return sm_leader(s);
   } else {
     // yay. all acceptors accepted
     s.depth++;
     s.state = S_ACCEPTED_PROPOSAL;
     s.nodes_left = -1;     
     return sm_leader(s);
   }
}

state sm_leader(state s) {
  printf("%*s" "LEADER - state %d node %d nodes_left %d ticket %ld type %d slot %ld value %ld\n", 
	 s.depth, "", s.state, s.node_num, s.nodes_left, s.ticket, s.type, s.slot, s.value);
  state latest_state = s;
  switch(s.state) {
  case S_AVAILABLE:
    latest_state = sm_leader_available(s);
    break;
  case S_PREPARE:
    latest_state = sm_leader_prepare(s);
    break;
  case S_SEND_PROPOSAL_TO_ACCEPTOR:
    latest_state = sm_leader_send_proposal(s);
    break;
  case S_COLLECT_ACCEPTOR_PROPOSAL_RESPONSE:
    latest_state = sm_leader_collect(s);
    break;
  case S_ACCEPTED_PROPOSAL:
    ;
    // send value to all nodes
    send_to(s.node_num % num_nodes, s.ticket, LEARN_THIS, s.slot, s.value);
    break;
  default:
    assert(0);
    break;
  }
  return latest_state;
}

void intheory_sm(enum role_t role) {
  state s;

  s.type = s.node_num = s.nodes_left = s.slot = s.value = -1;
  s.ticket = 0;
  s.depth = 0; 
  switch(role) {
  case LEADER:
    s.state = S_AVAILABLE;
    sm_leader(s);
    break;
  case ACCEPTOR:
    break;
  case LEARNER:
    break;  
  case CLIENT:
    break;
  }
}

