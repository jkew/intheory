#ifndef INTHEORY_UTIL
#define INTHEORY_UTIL
#include <stdlib.h>
#define TRUE 1
#define FALSE 0
typedef int bool;


bool deadline_passed(unsigned long deadline);
unsigned long get_deadline(unsigned long ms);
void *create(size_t size);
void discard(void *thing);

#endif
