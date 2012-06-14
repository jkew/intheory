#ifndef INTHEORYNETWORKH
#define INTHEORYNETWORKH
#include "intheory.h"

typedef struct {
  int type;
  int from;
  int to;
  long ticket;
  long slot;
  long value;
} message;

#define CLIENT_VALUE 1
#define PROPOSAL 2
#define ACCEPTED_PROPOSAL 4
#define REJECTED_PROPOSAL 8
#define SET 16
#define GET 32
#define EXIT 64

extern char ** nodes; 
extern int num_nodes;
extern message * (*recv_from)(int, long, unsigned int);
extern int (*send_to)(int, long, int, long, long);

void init();

#endif
