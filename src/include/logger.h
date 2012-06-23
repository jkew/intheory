#ifndef ITLOGGERH
#define ITLOGGERH
#include "intheory.h"
#include "state_machine.h"
#include "network.h"

void log_state(state s, enum role_t r);
void log_message(char *msg, message *m);
void log_graph(int from_node, int to_node, int message, int recv);
void error(const char *msg, ...);
void info(const char *msg, ...);
void notice(const char *msg, ...);
void trace(const char *msg, ...);
const char * getMessageName(int);

#endif
