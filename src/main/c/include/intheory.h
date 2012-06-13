#ifndef INTHEORYH
#define INTHEORYH

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

void intheory_sm(enum role_t role);

#endif
