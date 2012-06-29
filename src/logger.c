#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "include/intheory.h"
#include "include/network.h"
#include "include/state_machine.h"
#include "include/logger.h"

int log_level = TRACE;

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
  case S_CLIENT_RESPOND: return "S_CLIENT_RESPOND";
  case S_DONE: return "S_DONE";
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

void set_log_level(enum loglevel_t level) {
  log_level = level;
}

void error(const char *msg, ...) {
  if (log_level < ERROR) return;
  va_list args;
  fprintf(stderr, "ERROR: ");
  va_start( args, msg );
  vfprintf(stderr, msg, args );
  va_end( args );
  fprintf(stderr, "\n" );
}

void info(const char *msg, ...) {
  if (log_level < INFO) return;
  va_list args;
  fprintf(stdout, "INFO:");
  va_start( args, msg );
  vfprintf(stdout, msg, args );
  va_end( args );
  fprintf(stdout, "\n" );
}

void trace(const char *msg, ...) {
  if (log_level < TRACE) return;
  va_list args;
  fprintf(stdout, "TRACE:");
  va_start( args, msg );
  vfprintf(stdout, msg, args );
  va_end( args );
  fprintf(stdout, "\n" );
}

void notice(const char *msg, ...) {
  if (log_level < NOTICE) return;
  va_list args;
  fprintf(stdout, "NOTICE:");
  va_start( args, msg );
  vfprintf(stdout, msg, args );
  va_end( args );
  fprintf(stdout, "\n" );
}

void log_message(char *msg, message *m) {
  trace("(%s) Message %s from %d to %d ticket %ld slot %ld value %ld crc 0x%lx", msg, getMessageName(m->type), m->from, m->to, m->ticket, m->slot, m->value, m->crc);
}

int message_to_role(int m) {
  switch(m) {
  case CLIENT_VALUE: return PROPOSER;
  case PROPOSAL: return ACCEPTOR;
  case ACCEPTED_PROPOSAL: return PROPOSER;
  case REJECTED_PROPOSAL: return PROPOSER;
  case ACCEPTOR_SET: return ACCEPTOR;
  case SET: return LEARNER;
  case GET: return LEARNER;
  case READ_SUCCESS: return CLIENT;
  case READ_FAILED: return CLIENT;
  case WRITE_SUCCESS: return CLIENT;
  case WRITE_FAILED: return CLIENT;
  case EXIT: return PROPOSER; // technically wrong
  default: return PROPOSER;
  };
}

int message_from_role(int m) {
  switch(m) {
  case CLIENT_VALUE: return CLIENT;
  case PROPOSAL: return PROPOSER;
  case ACCEPTED_PROPOSAL: return ACCEPTOR;
  case REJECTED_PROPOSAL: return ACCEPTOR;
  case ACCEPTOR_SET: return PROPOSER;
  case SET: return ACCEPTOR;
  case GET: return CLIENT;
  case READ_SUCCESS: return LEARNER;
  case READ_FAILED: return LEARNER;
  case WRITE_SUCCESS: return PROPOSER;
  case WRITE_FAILED: return PROPOSER;
  case EXIT: return PROPOSER; // technically wrong
  default: return PROPOSER;
  };
}

void draw_base_graph(char *line, int line_size) {
  memset(line, ' ', line_size);
  line[line_size - 1] = 0;
  int i;
  for (i = 0; i < num_nodes(); i++) {
    line[graph_index(i, PROPOSER)] = '|';
    line[graph_index(i, ACCEPTOR)] = '|';
    line[graph_index(i, LEARNER)] = '|';
    line[graph_index(i, CLIENT)] = '|';
  }
}

int graph_index(int node, enum role_t role) {
  return node * 9 + role * 2;
}

void log_graph(int from_node, int to_node, int message, int recv) {
  if (log_level < GRAPH) return;
  enum role_t to_role = message_to_role(message);
  enum role_t from_role = message_from_role(message);

  int line_size = (num_nodes()) * 9 + 6;

  char line[line_size];
  draw_base_graph(line, line_size);

  // printf("GRAPH(0): %s %s\n", line, getMessageName(message));
  int to_idx = graph_index(to_node, to_role);
  int from_idx = graph_index(from_node, from_role);

  line[from_idx] = '*';
  line[to_idx] = 'M';

  if (to_idx < from_idx) {
    int tmp = to_idx;
    to_idx = from_idx;
    from_idx = tmp;
    line[to_idx - 1] = '-';
    line[from_idx + 1] = '<';
  } else {
    line[from_idx + 1] = '-';
    line[to_idx - 1] = '>';
  }

  int i;
  for (i = (from_idx + 2); i < (to_idx - 1); i++) {
    line[i] = '-';
  }

  printf("GRAPH: %s %s %s\n", line, recv ? "RECV " : "SENT ", getMessageName(message));
  
}

void log_state(state s, enum role_t r) {
  if (log_level < GRAPH) return;
  int line_size = (num_nodes()) * 9 + 6;
  char line[line_size];
  draw_base_graph(line, line_size);

  int meidx = graph_index(my_id(), r);

  line[meidx] = getStateName(s.state)[2];

  printf("GRAPH: %s STATE %s %s %ld %ld %ld\n", line, getRole(r), getStateName(s.state), s.ticket, s.slot, s.value);
}
