#include <stdlib.h>
#include "include/state_machine.h"

int deadline_passed(struct timeval *deadline) {
  struct timeval current;
  gettimeofday(&current, 0);
  if (current.tv_sec > deadline->tv_sec) {    
    return 1;
  }
  return 0;
}

void set_deadline(int sec, struct timeval *deadline) {
  struct timeval current;
  struct timeval duration;
  gettimeofday(&current, 0);
  duration.tv_sec = sec;
  duration.tv_usec = 0;
  timeradd(&current, &duration, deadline);
}

void * create(size_t s) {
  return malloc(s);
}

void discard(void *thing) {
  free(thing);
}
