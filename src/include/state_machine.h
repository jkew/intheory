#ifndef STATEMACHINEH
#define STATEMACHINEH

// TODO: Eventually remove this limitation
#define MAX_QUOROM_SIZE 16
#include <sys/time.h>

enum role_t {
  PROPOSER,
  ACCEPTOR,
  LEARNER,
  CLIENT,
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
  S_CLIENT_RESPOND,
  S_DONE,
};

typedef struct {
  unsigned short state; 
  short type;
  unsigned short nodes_quorom[MAX_QUOROM_SIZE]; // replace with list
  unsigned short num_quorom;
  unsigned short max_fails;
  unsigned short fails;
  short nodes_left;
  unsigned short flags;
  long ticket;
  int slot;
  long value;
  unsigned short client;
  unsigned long deadline;
} state;

typedef state (*sm_role_fn)(state);

extern unsigned long deadline;

state init_state(enum role_t role, state *prev_state);
void init_sm();
void next_states();
#endif
