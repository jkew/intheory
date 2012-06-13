#ifndef INTHEORYNETWORKH
#define INTHEORYNETWORKH
#include <intheory.h>

char ** nodes = 0; 
int num_nodes = -1;

message * (*recv_from)(int, long, int) = 0;
int (*send_to)(int, long, enum message_t, long, long) = 0;

void init();

#endif
