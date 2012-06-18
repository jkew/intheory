#ifndef STATEMACHINEH
#define STATEMACHINEH

// proposer has at most 5 active states
#define MAX_STACK_SIZE 5

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
  S_DONE
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
  long fails;
} state;

typedef struct {
  state state_stack[MAX_STACK_SIZE];
  int size;
  enum role_t role;
} stack; 

typedef state (*sm_role_fn)(state);

void intheory_sm(enum role_t role);
state init_state(enum role_t role);
stack init_stack(enum role_t role);

#endif
