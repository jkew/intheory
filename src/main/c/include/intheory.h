#ifndef INTHEORYH
#define INTHEORYH

enum role_t {
  PROPOSER,
  ACCEPTOR,
  CLIENT,
  LEARNER
};

enum state_t {
  S_AVAILABLE,
  S_PREPARE,
  S_SEND_PROPOSAL_TO_ACCEPTOR,
  S_COLLECT_ACCEPTOR_PROPOSAL_RESPONSE,
  S_ACCEPT_PROPOSAL,
  S_ACCEPTED_PROPOSAL,
  S_ACCEPTED_WAIT,
  S_SET,
  S_GET,
};

typedef struct {
  int state; 
  int type;
  int depth; 
  int node_num;
  int nodes_left;
  long ticket;
  long slot;
  long value;
} state;

void intheory_sm(enum role_t role);
void discard(void *thing);

#endif
