#include <stdlib.h>
#include "include/state_machine.h"

int deadline_passed(state *s) {
  struct timeval current;
  gettimeofday(&current, 0);

  if (current.tv_sec > s->deadline.tv_sec) {
    trace("Deadline Passed to %d usec %d, Current %d usec %d", s->deadline.tv_sec, s->deadline.tv_usec, current.tv_sec, current.tv_usec);
    return 1;
  }

  return 0;
}

void set_deadline(int sec, state *s) {
  struct timeval current;
  struct timeval duration;
  gettimeofday(&current, 0);
  duration.tv_sec = sec;
  duration.tv_usec = 0;
  timeradd(&current, &duration, &(s->deadline));
  trace("Deadline set to %d usec %d, Current %d usec %d", s->deadline.tv_sec, s->deadline.tv_usec, current.tv_sec, current.tv_usec);
}

void discard(void *thing) {
  free(thing);
}
