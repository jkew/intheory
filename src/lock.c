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
  int heldby;
  long deadline;
} lock_t;

list_ref *locks;

void init_locks() {
  locks = new_list();
}

void destroy_locks() {
  lock_t *delete = NULL;
  while(delete = popv(locks)) { free(delete); }
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
  // find the lock in the lock list;
  listi itr = find_lock(slot);
  lock_t *l = (lock_t *)value_itr(itr);
  l->heldby = value;
  if (l != NULL && l->slot == slot) {
    switch(op) {
    case SLOT_CREATE:      
    case SLOT_UPDATE:
      l->heldby = value;
      if (value != -1) return;
    case SLOT_DELETE:
      acquire_lock(l);
    }
  } 
}

void maintain_locks() {
  // TODO: call store.expire
  expire_slots();
  // TODO: loop through locks; any which are half-way to
  // expired reset
  listi itr = list_itr(locks);
  while(there(itr)) {
    lock_t *l = (lock_t *)value_itr(itr);
    if (l->heldby == my_id()) {
      // and deadline expired; reset
      set_it(l->slot, my_id(), LOCK | TIMEOUT | ASYNC_SEND);
    }
    itr = next_itr(itr);
  }
  
}

void lock(int slot) {
  int val;
  register_changed_cb(slot, lock_changed_cb);
  lock_t *l = malloc(sizeof(lock_t));
  l->slot = slot;
  l->heldby = -1;
  if (!get_it(slot, &(l->heldby))) {
    l->heldby = -1;
  }
  l->deadline = -1;
  pushv(locks, l);
  // wait for the lock to be aquired
  while(l->heldby != my_id()) {
    usleep(10000);
  }
}

void unlock(int slot) {
  listi itr = find_lock(slot);
  lock_t *delete = (lock_t *)value_itr(itr);
  if (delete != NULL && delete->slot == slot) {
    remove_itr(locks, itr, &delete);
    free(delete);
    set_it(slot, -1, UNLOCK);
  }
  unregister_cb(slot, lock_changed_cb);
}
