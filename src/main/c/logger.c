#include <stdio.h>
#include <stdlib.h>
#include "include/intheory.h"
#include "include/logger.h"

const char* getRole(enum role_t r) {
  switch(r) {
  case PROPOSER: return "PROPOSER";
  case ACCEPTOR: return "ACCEPTOR";
  case CLIENT: return "CLIENT";
  case LEARNER: return "LEARNER";
  };
}

const char* getStateName(int i) {
  switch(i) {
  case S_AVAILABLE: return "S_AVAILABLE";
  case S_PREPARE: return "S_PREPARE";
  case S_SEND_PROPOSAL_TO_ACCEPTOR: return "S_SEND_PROPOSAL_TO_ACCEPTOR";
  case S_COLLECT_ACCEPTOR_PROPOSAL_RESPONSE: return "S_COLLECT_ACCEPTOR_PROPOSAL_RESPONSE";
  case S_ACCEPT_PROPOSAL: return "S_ACCEPT_PROPOSAL";
  case S_ACCEPTED_WAIT: return "S_ACCEPTED_WAIT";
  case S_ACCEPTED_PROPOSAL: return "S_ACCEPTED_PROPOSAL";
  case S_SET: return "S_SET";
  case S_GET: return "S_GET";
  default: return "UNKNOWN";
  }
}

void error(const char *msg) {
  printf("ERROR: %s\n", msg); 
}

void info(const char * msg) {
  printf("INFO: %s\n", msg); 
}

void log_state(state s, enum role_t r) {
  printf("%*s" "%s - %s node %d nodes_left %d ticket %ld type %d slot %ld value %ld\n", 
	 s.depth, "", getRole(r), getStateName(s.state), s.node_num, s.nodes_left, s.ticket, s.type, s.slot, s.value);
}
