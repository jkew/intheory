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

typedef struct {
  int state; 
  enum message_t type;
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
