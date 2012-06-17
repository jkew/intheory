#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "include/intheory.h"
#include "include/network.h"
#include "include/state_machine.h"
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

const char* getMessageName(int i) {
  switch(i) {
  case CLIENT_VALUE: return "CLIENT_VALUE";
  case PROPOSAL: return "PROPOSAL";
  case ACCEPTED_PROPOSAL: return "ACCEPTED_PROPOSAL";
  case REJECTED_PROPOSAL: return "REJECTED_PROPOSAL";
  case ACCEPTOR_SET: return "ACCEPTOR_SET";
  case SET: return "SET";
  case GET: return "GET";
  case READ_SUCCESS: return "READ_SUCCESS";
  case READ_FAILED: return "READ_FAILED";
  case WRITE_SUCCESS: return "WRITE_SUCCESS";
  case WRITE_FAILED: return "WRITE_FAILED";
  case EXIT: return "EXIT";
  default: return "UNKNOWN";
  }
}

void error(const char *msg, ...) {
  va_list args;
  fprintf(stderr, "ERROR: ");
  va_start( args, msg );
  vfprintf(stderr, msg, args );
  va_end( args );
  fprintf(stderr, "\n" );
}

void info(const char *msg, ...) {
  return;
  va_list args;
  fprintf(stdout, "INFO:");
  va_start( args, msg );
  vfprintf(stdout, msg, args );
  va_end( args );
  fprintf(stdout, "\n" );
}

void log_state(state s, enum role_t r) {
  info("%*s" "%s - %s node %d nodes_left %d ticket %ld type %d slot %ld value %ld", 
	 s.depth, "", getRole(r), getStateName(s.state), s.node_num, s.nodes_left, s.ticket, s.type, s.slot, s.value);
}

void log_message(char *msg, message *m) {
  info("(%s) Message %s from %d to %d ticket %ld slot %ld value %ld crc 0x%lx", msg, getMessageName(m->type), m->from, m->to, m->ticket, m->slot, m->value, m->crc);
}
