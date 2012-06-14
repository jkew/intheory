#ifndef ITLOGGERH
#define ITLOGGERH
#include "intheory.h"

void log_state(state s, enum role_t r);
void error(const char *msg);
void info(const char *msg);

#endif
