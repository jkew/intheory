#ifndef ITTESTCOMMONH
#define ITTESTCOMMONH
#include "../../src/include/intheory.h"
#include "../../src/include/network.h"
#include "../../src/include/state_machine.h"

#define SLOT 0
#define COUNT_TO 10
#define TOTAL_NODES 4

extern long (*recv)[6];
extern long (*send)[6];
extern int sendidx;
extern int recvidx;

extern int _node_count;
extern int running;
message * recv_from_scenario(int, long, unsigned int);
int send_to_scenario(int, long, unsigned int, long, long);
void got_value(int slot, long value, unsigned short op);
int spawn_nodes(char *all_nodes);
void intheory_sm(enum role_t role);
#endif
