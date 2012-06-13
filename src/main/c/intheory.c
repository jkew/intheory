#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

enum role_t {
  LEADER,
  ACCEPTOR,
  CLIENT,
  LEARNER
};

enum state_t {
  S_AVAILABLE,
  S_PREPARE,
  S_SEND_PROPOSAL_TO_ACCEPTOR,
  S_COLLECT_ACCEPTOR_PROPOSAL_RESPONSE,
  S_ACCEPTED_PROPOSAL,
};

enum message_t {
  CLIENT_VALUE,
  PROPOSAL,
  ACCEPTED_PROPOSAL,
  REJECTED_PROPOSAL,
  LEARN_THIS,
  EXIT,
};

typedef struct {
  enum message_t type;
  long ticket;
  long slot;
  long value;
} message;

//typedef struct
//int depth, int state, int node_num, int nodes_left, long ticket, enum message_t type, long slot, long value

char ** nodes = NULL; 
int num_nodes = -1;

message * (*recv_from)(int, long, int) = NULL;
int (*send_to)(int, long, enum message_t, long, long) = NULL;

message * recv_from_network(int from_node, long slot, enum message_t expected) {
  return NULL;
}

int send_to_network(int node, long ticket, enum message_t type, long slot, long value) {
  return -1;  
}

void discard(void *thing) {
  printf("   free %p\n", thing);
  free(thing);
}

long sm_leader(int depth, int state, int node_num, int nodes_left, long ticket, enum message_t type, long slot, long value) {
  printf("%*s" "LEADER - state %d node %d nodes_left %d ticket %ld type %d slot %ld value %ld\n", 
	 depth, "", state, node_num, nodes_left, ticket, type, slot, value);
  long latest_ticket = ticket;
  switch(state) {
  case S_AVAILABLE:
    assert(nodes_left = -1 && node_num == -1 && ticket >= 0 && type == -1 && slot == -1 && value == -1);
    message* mesg = recv_from(-1, -1, CLIENT_VALUE);
    latest_ticket = sm_leader(++depth, S_PREPARE, -1, -1, ticket, CLIENT_VALUE, mesg->slot, mesg->value);
    discard(mesg);
    break;
  case S_PREPARE:
    assert(nodes_left = -1 && node_num == -1 && ticket >= 0 && type == CLIENT_VALUE && slot >= 0 && value >= 0);
    // send to a quorom of acceptors
    int num_acceptors = ((int) num_nodes / 2) + 1;
    int start_node = random() % num_nodes;
    latest_ticket = sm_leader(++depth, S_SEND_PROPOSAL_TO_ACCEPTOR, start_node, num_acceptors, ticket + 1, PROPOSAL, slot, value);
    break;
  case S_SEND_PROPOSAL_TO_ACCEPTOR:
    assert(nodes_left > 0 && node_num >= 0 && ticket >= 0 && type == PROPOSAL && slot >= 0 && value >= 0);
    int to_node = node_num % num_nodes;
    while (!send_to(to_node, ticket, type, slot, value)) {
      to_node = (to_node++) % num_nodes;
    }
    latest_ticket = sm_leader(++depth, S_COLLECT_ACCEPTOR_PROPOSAL_RESPONSE, to_node, nodes_left, ticket, type, slot, value);
    break;
  case S_COLLECT_ACCEPTOR_PROPOSAL_RESPONSE:
    assert(nodes_left > 0 && node_num >= 0 && ticket >= 0 && type == PROPOSAL && slot >= 0 && value >= 0);
    message* response = recv_from(node_num, slot, -1); // really want to filter by ACCEPTED_PROPOSAL or REJECTED HERE
    if (response == NULL) { // failed to receive, try again with a new node
      printf("! Failed to recieve message from acceptor\n");
      latest_ticket = sm_leader(++depth, S_SEND_PROPOSAL_TO_ACCEPTOR, node_num + 1, nodes_left, ticket, type, slot, value);
      break; 
    }
    assert(response->slot == slot);
    if (response->type == REJECTED_PROPOSAL) { // rejected directly, propose with a new ticket
      printf("! Rejected proposal message from acceptor\n");
      // delay?
      latest_ticket = sm_leader(++depth, S_PREPARE, -1, -1, ticket, CLIENT_VALUE, slot, value);
      break;
    }
    assert(response->type == ACCEPTED_PROPOSAL);
    long new_value = value;
    if (response->ticket > ticket) {
      printf("? Recieved updated ticket from acceptor\n");
      new_value = response->value;
      latest_ticket = response->ticket;
    }
    if (nodes_left > 1) {
      // continue to contact acceptors
      printf("? nodes left to propose to\n");
      latest_ticket = sm_leader(++depth, S_SEND_PROPOSAL_TO_ACCEPTOR, node_num + 1, nodes_left - 1, latest_ticket, type, slot, new_value);
      latest_ticket = sm_leader(++depth, S_ACCEPTED_PROPOSAL, node_num, -1, latest_ticket, type, slot, new_value);
    } else {
      // yay. all acceptors accepted
      latest_ticket = sm_leader(++depth, S_ACCEPTED_PROPOSAL, node_num, -1, latest_ticket, type, slot, new_value);
    }
    break;
  case S_ACCEPTED_PROPOSAL:
    ;
    // send value to all nodes
    send_to(node_num % num_nodes, ticket, LEARN_THIS, slot, value);
    break;
  default:
    assert(0);
    break;
  }
  return latest_ticket;
}

void sm(enum role_t role) {
  switch(role) {
  case LEADER:
    sm_leader(0, S_AVAILABLE, -1, -1, 0, -1, -1, -1);
    break;
  case ACCEPTOR:
    break;
  case LEARNER:
    break;  
  case CLIENT:
    break;
  }
}

void test_basic();

void main(int argc, char **args) {
  test_basic();
  //sm(LEADER);
}

long (*recv)[4];
long (*send)[4];

/**
 * PROPOSER SCENARIO: Basic
 **/
long recv1[][4] = { 
  { CLIENT_VALUE, -1, 0, 999},
  { ACCEPTED_PROPOSAL, 1, 0, -1},
  { ACCEPTED_PROPOSAL, 1, 0, -1},
  { ACCEPTED_PROPOSAL, 1, 0, -1}
};

long send1[][4] = {
  { PROPOSAL, 1, 0, 999},
  { PROPOSAL, 1, 0, 999},
  { PROPOSAL, 1, 0, 999},
  { LEARN_THIS, 1, 0, 999},
  { LEARN_THIS, 1, 0, 999},
  { LEARN_THIS, 1, 0, 999},
  { LEARN_THIS, 1, 0, 999}
};

/**
 * PROPOSER SCENARIO: FAILURE OF ACCEPTOR
 **/
long recv2[][4] = { 
  { CLIENT_VALUE, -1, 0, 999},
  { ACCEPTED_PROPOSAL, 1, 0, -1},
  { ACCEPTED_PROPOSAL, 1, 0, -1},
  { -1, -1, -1, -1},
  { ACCEPTED_PROPOSAL, 1, 0, -1},
  { ACCEPTED_PROPOSAL, 1, 0, -1},
  { ACCEPTED_PROPOSAL, 1, 0, -1},
};

long send2[][4] = {
  { PROPOSAL, 1, 0, 999},
  { PROPOSAL, 1, 0, 999},
  { PROPOSAL, 1, 0, 999},
  { PROPOSAL, 1, 0, 999},
  { LEARN_THIS, 1, 0, 999},
  { LEARN_THIS, 1, 0, 999},
  { LEARN_THIS, 1, 0, 999},
  { LEARN_THIS, 1, 0, 999}
};

/**
 * PROPOSER SCENARIO: Acceptor returns value with newer ticket
 */
long recv3[][4] = { 
  { CLIENT_VALUE, -1, 0, 999},
  { ACCEPTED_PROPOSAL, 1, 0, -1},
  { ACCEPTED_PROPOSAL, 1, 0, -1},
  { ACCEPTED_PROPOSAL, 2, 0, 888},
};

long send3[][4] = {
  { PROPOSAL, 1, 0, 999},
  { PROPOSAL, 1, 0, 999},
  { PROPOSAL, 1, 0, 999},
  { LEARN_THIS, 2, 0, 888},
  { LEARN_THIS, 2, 0, 888},
  { LEARN_THIS, 2, 0, 888}
};

int sendidx = 0;
int recvidx = 0;

message * recv_from_scenario(int from_node, long slot, enum message_t expected) {
  if (recv[recvidx][0] == -1) { 
    recvidx++;
    return 0;
  }
  message *msg; 
  msg = malloc(sizeof(message));
  msg->type = recv[recvidx][0];
  msg->ticket = recv[recvidx][1];
  msg->slot = recv[recvidx][2];
  msg->value = recv[recvidx][3];
  printf("< Recv - %d - ticket %ld type %d slot %ld value %ld\n", 
	 recvidx, msg->ticket, msg->type, msg->slot, msg->value);
  recvidx++;  
  return msg;
}

int send_to_scenario(int node, long ticket, enum message_t type, long slot, long value) {
  printf("> Send - %d - ticket %ld type %d slot %ld value %ld\n", 
	 sendidx, ticket, type, slot, value);
  assert(type == send[sendidx][0]);
  assert(ticket == send[sendidx][1]);
  assert(slot == send[sendidx][2]);
  assert(value == send[sendidx][3]);
  sendidx++;
  return 1;
}

void test_basic() {
  num_nodes = 4;
  recv = recv1;
  send = send1;
  recv_from = &recv_from_scenario; 
  send_to = &send_to_scenario; 
  sm(LEADER);
  recv = recv2;
  send = send2;
  sendidx = recvidx = 0;
  sm(LEADER);
  //recv = recv3;
  //send = send3;
  //sendidx = recvidx = 0;
  //sm(LEADER);

} 
