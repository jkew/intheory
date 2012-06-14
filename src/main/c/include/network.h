#ifndef INTHEORYNETWORKH
#define INTHEORYNETWORKH
#include "intheory.h"

extern char ** nodes; 
extern int num_nodes;
extern message * (*recv_from)(int, long, int);
extern int (*send_to)(int, long, enum message_t, long, long);

void init();

#endif
