#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include "include/util.h"
#include "include/store.h"
#include "include/logger.h"
#include "include/callbacks.h"

#define INITIAL_SLOT_SIZE 256

long *slots = 0;
int maxslot = 0;

void init_store() {
  slots = malloc(sizeof(long)*INITIAL_SLOT_SIZE);
  maxslot = 255;
}

void destroy_store() {
  discard(slots);
  slots = 0;
  maxslot = 0;
}

void set(int slot, long value) {
  if (slot > maxslot) {
    int new_size = slot * 2;
    long *oldslots = slots;
    long *newslots = malloc(sizeof(long)*new_size);

    memcpy(newslots, oldslots, maxslot + 1);    
    slots = newslots;
    maxslot = new_size - 1;
    discard(oldslots);
  }
  long oldval = slots[slot];
  slots[slot] = value;
  slot_changed(slot, value);
}

long get(int slot) {
  return slots[slot];
}

int get_max_slot() {
  return maxslot;
}
