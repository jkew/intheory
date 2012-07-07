#include <stdlib.h>
#include "include/util.h"

long timeval_to_ms(struct timeval tv) {
  return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

bool deadline_passed(long deadline_ms) {
  struct timeval current;
  gettimeofday(&current, 0);
  long current_ms = timeval_to_ms(current);
  if (current_ms > deadline_ms) {    
    return 1;
  }
  return 0;
}

long get_deadline(long ms) {
  struct timeval current;
  gettimeofday(&current, 0);
  long current_ms = timeval_to_ms(current);
  return current_ms + ms;
}

void * create(size_t s) {
  return malloc(s);
}

void discard(void *thing) {
  free(thing);
}
