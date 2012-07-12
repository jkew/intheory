#include <stdlib.h>
#include <assert.h>
#include "include/util.h"

unsigned long timeval_to_ms(struct timeval *tv) {
  unsigned long sec_ms = ((unsigned long)tv->tv_sec) * 1000;
  unsigned long usec_ms = ((unsigned long)tv->tv_usec) / 1000;
  return sec_ms + usec_ms;
}

bool deadline_passed(unsigned long deadline_ms) {
  struct timeval current;
  gettimeofday(&current, 0);
  unsigned long current_ms = timeval_to_ms(&current);
  //assert(current_ms < (deadline_ms * 2));
  if (current_ms > deadline_ms) {
    return 1;
  }
  return 0;
}

unsigned long get_deadline(unsigned long ms) {
  struct timeval current;
  gettimeofday(&current, 0);
  unsigned long current_ms = timeval_to_ms(&current);
  unsigned long deadline_result = current_ms + ms;
  return deadline_result;
}

void * create(size_t s) {
  return malloc(s);
}

void discard(void *thing) {
  free(thing);
}
