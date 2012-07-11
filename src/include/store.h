#ifndef ITSTOREH
#define ITSTOREH
#include "intheory.h"
#include "util.h"

// Initializes the slot store
void init_store();
// Destroys the slot store
void destroy_store();
// Returns the value of the slot or 0 if the slot is not set
long get(int slot);
// Returns TRUE if the slot exists, FALSE otherwise
bool exists(int slot);
// Sets a value to a slot, with the given deadline in ms
// if the deadline is negative the value will be set indefinitely
void set(int slot, long value, long deadline_ms);
// Gets the maximum slot value
int get_max_slot();

#endif
