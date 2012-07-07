#ifndef ITSTOREH
#define ITSTOREH
#include "intheory.h"

void init_store();
void destroy_store();
long get(int slot);
void set(int slot, long value, long deadline);
int get_max_slot();

#endif
