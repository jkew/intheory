#ifndef INTHEORY_UTIL
#define INTHEORY_UTIL

int deadline_passed(struct timeval *deadline);
void set_deadline(int sec, struct timeval *deadline);
void *create(size_t size);
void discard(void *thing);

#endif
