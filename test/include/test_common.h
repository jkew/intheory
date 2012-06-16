#ifndef ITTESTCOMMONH
#define ITTESTCOMMONH
#include "../../src/include/intheory.h"
#include "../../src/include/network.h"
extern long (*recv)[5];
extern long (*send)[5];
extern int sendidx;
extern int recvidx;

message * recv_from_scenario(int, long, unsigned int);
int send_to_scenario(int, long, unsigned int, long, long);
#endif
