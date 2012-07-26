#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include "include/util.h"
#include "include/store.h"
#include "include/list.h"
#include "include/logger.h"
#include "include/callbacks.h"


typedef struct {
  int slot;
  long value;
  long deadline;
} slot_val;

list_ref *slots;
pthread_mutex_t store_lock;

void init_store() {
  slots = new_list();
}

void destroy_store() {
  pthread_mutex_lock(&store_lock);
  slot_val *delete = NULL;
  while(delete = popv(slots)) { free(delete); }
  free(slots);
  pthread_mutex_unlock(&store_lock);
}

bool verify(slot_val *s) {
  assert(s != NULL);
  if (s->deadline > 0 && deadline_passed(s->deadline)) {
    return FALSE;
  }
  return TRUE;
}

void expire_slots() {
  pthread_mutex_lock(&store_lock);
  listi itr = list_itr(slots);
  while(there(itr)) {
    slot_val *curr = (slot_val *)value_itr(itr);
    if (!verify(curr)) {
      slot_val *delete;
      itr = remove_itr(slots, itr, &delete);
      assert(curr == delete);
      discard(curr);
      curr = NULL;
      continue;
    }
    itr = next_itr(itr);
  }
  pthread_mutex_unlock(&store_lock);
}

listi find(int slot) {
  listi itr = list_itr(slots);
  while(there(itr)) {
    slot_val *curr = (slot_val *)value_itr(itr);
    if (curr->slot > slot) {      
      return prev_itr(itr);
    }
    if (curr->slot == slot) {
      return itr;
    }
    itr = next_itr(itr);
  }
  return itr;
}

void set(int slot, long value, long deadline) {
  pthread_mutex_lock(&store_lock);
  listi itr = find(slot);
  slot_val *s = value_itr(itr);
  if (s == NULL || s->slot != slot) {
    s = malloc(sizeof(slot_val));
    s->slot = slot;
    if (there(itr)) {
    } else {
    }

    add_itr(slots, itr, s);
  }
  s->value = value;
  s->deadline = deadline;
  pthread_mutex_unlock(&store_lock);
  slot_changed(slot, value);
}

bool exists(int slot) {
  pthread_mutex_lock(&store_lock);
  listi itr = find(slot);
  bool ret = FALSE;
  if (there(itr) && ((slot_val *)value_itr(itr))->slot == slot) {
    ret = TRUE;
  }
  pthread_mutex_unlock(&store_lock);
  return ret;
}

long get(int slot) {
  pthread_mutex_lock(&store_lock);
  listi itr = find(slot);
  slot_val *s = value_itr(itr);
  if (s != NULL && s->slot == slot) { 
    pthread_mutex_unlock(&store_lock);
    return s->value;
  }
  pthread_mutex_unlock(&store_lock);
  return 0;
}
