#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include "include/util.h"
#include "include/store.h"
#include "include/logger.h"
#include "include/callbacks.h"

typedef struct {
  int slot;
  long value;
  long deadline;
  struct slot *prev;
  struct slot *next;
} slot_node;


slot_node *slots;

void init_store() {
}

void destroy_store() {
  while(slots != NULL) {
    remove_slot(slots);
  }
}

void remove_slot(slot_node *s) {
  slot_node *next, *prev;
  next = s->next;
  prev = s->prev;

  if (prev == NULL) {
    slots = next;
    if (next != NULL) {
      next->prev = NULL;
    }
  } else {
    prev->next = next;
  }
  discard(s);
}

void add_slot(slot_node *s, slot_node *prev) {
  if (prev != NULL) { 
    if (prev->next != NULL) {
      s->next = prev->next;
      ((slot_node *)prev->next)->prev = s;
    }
    s->prev = prev; 
    prev->next = s; 
  } else {
    s->next = NULL;
    slots = s;
  }
}

bool verify(slot_node *s) {
  assert(s != NULL);
  slot_node *ret = s;
  if (s->deadline > 0 && deadline_passed(s->deadline)) {
    return FALSE;
  }
  return TRUE;
}

slot_node *find(int slot) {
  slot_node *curr = slots;
  slot_node *ret = NULL;
  while(curr != NULL) {
    if (!verify(curr)) {
      slot_node *old = curr;
      curr = curr->next;
      remove_slot(old);
      continue;
    }
    if (curr->slot == slot) {
      ret = curr;
      return ret;
    }
    if (curr->slot > slot) {
      curr = curr->prev;
      break;
    }
    curr = curr->next;
  }
  return ret;
}

void set(int slot, long value, long deadline) {
  
  slot_node *s = find(slot);
  if (s == NULL || s->slot != slot) {
    slot_node *curr = malloc(sizeof(slot_node));
    curr->slot = slot;
    curr->prev = NULL;
    curr->next = NULL;
    add_slot(curr, s);
    s = curr;
  }
  s->value = value;
  s->deadline = deadline;
  slot_changed(slot, value);
}

bool exists(int slot) {
  slot_node *s = find(slot);
  if (s != NULL && s->slot == slot) return TRUE;
  return FALSE;
}

long get(int slot) {
  slot_node *s = find(slot);
  if (s != NULL && s->slot == slot) return s->value;
  return 0;
}
