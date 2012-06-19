#ifndef ITLOGGERH
#define ITLOGGERH
#include "intheory.h"

enum loglevel_t {
  NONE,
  ERROR,
  NOTICE,
  INFO,
  TRACE,
};

void log_state(state s, enum role_t r);
void log_message(char *msg, message *m);
void error(const char *msg, ...);
void info(const char *msg, ...);
void notice(const char *msg, ...);
void trace(const char *msg, ...);
const char * getMessageName(int);
#endif
