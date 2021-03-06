#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "include/intheory.h"
#include "include/lock.h"
#include "include/util.h"
#include "include/list.h"
#include "include/logger.h"
#include "include/callbacks.h"

typedef struct {
  int slot;
  long heldby;
  long deadline;
} lock_t;

list_ref *locks;

void init_locks() {
  locks = new_list();
}

void destroy_locks() {
  lock_t *delete = NULL;
  listi itr = list_itr(locks);
  while(there(itr)) {
    lock_t *l = (lock_t *)value_itr(itr);
    unlock(delete->slot);
    itr = list_itr(locks);
  }
  free(locks);
}

listi find_lock(int slot) {
  listi itr = list_itr(locks);
  while(there(itr)) {
    lock_t *l = (lock_t *)value_itr(itr);
    if (l->slot == slot) {
      return itr;
    }
    itr = next_itr(itr);
  }
  return itr;
}

void acquire_lock(lock_t *l) {
  set_it(l->slot, my_id(), LOCK | TIMEOUT | ASYNC_SEND);
}

void lock_changed_cb(int slot, long value, unsigned short op) {
  trace("LOCK %d changed status to %ld flags %d", slot, value, op);
  // find the lock in the lock list;
  listi itr = find_lock(slot);
  lock_t *l = (lock_t *)value_itr(itr);
  assert(l != NULL);
  l->heldby = value;
  if (l != NULL && l->slot == slot) {
    switch(op) {
    case SLOT_CREATE:
    case SLOT_UPDATE:
      l->heldby = value;
      if (value != -1) return;
      if (value == my_id()) return;
    case SLOT_DELETE:
      acquire_lock(l);
    }
  } 
}

void maintain_locks() {
  // loop through locks; any which are half-way to
  // expired reset
  listi itr = list_itr(locks);
  while(there(itr)) {
    lock_t *l = (lock_t *)value_itr(itr);
    if (l->heldby == my_id()) {
      // and deadline half-way to expired; reset
      if (deadline_passed(l->deadline - (deadline / 2)))
	set_it(l->slot, my_id(), LOCK | TIMEOUT | ASYNC_SEND);
    } else {
      // We could try to get the lock here as well, in a
      // synchronous way but we assume that the callbacks
      // registered for a particular slot work correctly.
    }
    itr = next_itr(itr);
  }
  
}

void lock(int slot) {
  int val;
  error("PREPARING TO LOCK %d", slot);
  register_changed_cb(slot, lock_changed_cb);
  lock_t *l = malloc(sizeof(lock_t));
  l->slot = slot;
  l->heldby = -1;
  l->deadline = -1;
  
  error("GETTING INITIAL VALUE %d", slot);
  if (!get_it(slot, (long *)&(l->heldby))) {
    l->heldby = -1;
  }

  if (l->heldby == -1) {
    error("IMMEDIATE ACQUIRE %d", slot);
    acquire_lock(l);
  }

  pushv(locks, l);
  // wait for the lock to be aquired
  error("WAITING FOR LOCK %d", slot);
  int last_held = -999;
  while(l->heldby != my_id()) {
    usleep(10000);
    if (l->heldby != last_held) {
      error("HELD BY %d", l->heldby);
      last_held = l->heldby;
    }
  }
  l->deadline = get_deadline(deadline);
  error("ACQUIRED LOCK %d", slot);
}

void unlock(int slot) {
  error("UNLOCK %d", slot);
  listi itr = find_lock(slot);
  lock_t *delete = (lock_t *)value_itr(itr);
  error("UNLOCK %d-0", slot);
  assert(delete != NULL);
  error("UNLOCK %d-1", slot);
  assert(delete->slot == slot);
  error("UNLOCK %d-2", slot);
  assert(delete->heldby == my_id());
  error("UNLOCK %d-3", slot);
  if (delete != NULL && delete->slot == slot) {
    error("UNLOCK %d-4", slot);
    remove_itr(locks, itr, &delete);
    error("UNLOCK %d-5", slot);
    free(delete);
    error("UNLOCK %d-6", slot);
    set_it(slot, -1, UNLOCK);
    error("UNLOCK %d-7", slot);
    unregister_cb(slot, lock_changed_cb);
  } 
  error("RELEASED LOCK %d", slot);
}
