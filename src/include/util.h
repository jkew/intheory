#ifndef INTHEORY_UTIL
#define INTHEORY_UTIL
#include <stdlib.h>
#define TRUE 1
#define FALSE 0
#define NULL 0
typedef int bool;


bool deadline_passed(long deadline);
long get_deadline(long ms);
void *create(size_t size);
void discard(void *thing);

#endif
