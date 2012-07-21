#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include "include/util.h"
#include "include/list.h"



void fsck(list_ref *list) {
  int realsize = 0;
  node *last = NULL;
  node *curr = list->head;
  if (curr != NULL) {
    realsize++;
    assert(curr->prev == NULL);
    assert(curr->value != NULL);    
  } else {
    return;
  }
  last = curr;
  curr = curr->next;
  while(curr != NULL) {
    realsize++;
    assert(curr->prev != NULL);
    assert(curr->prev == last);
    assert(curr->value != NULL);
    last = curr;
    curr = curr->next;
  }
  assert(realsize == sizel(list));
}

void add_node_after(list_ref *list, node *n, node *prev) {
  assert(list != NULL);
  assert(n != NULL);

  if (prev != NULL) { 
    if (prev->next != NULL) {
      n->next = prev->next;
      ((node *)prev->next)->prev = n;
    }
    n->prev = prev; 
    prev->next = n; 
  } else {
    if (list->head != NULL) {
      n->next = list->head;
      list->head->prev = n;
    } else {
      n->next = NULL;
    }
    list->head = n;
  }
  list->size++;
  //fsck(list);
}

void remove_node(list_ref *list, node *n) {
  assert(list != NULL);
  assert(n != NULL);

  node *next, *prev;
  next = n->next;
  prev = n->prev;

  if (prev == NULL) {
    list->head = next;
    if (next != NULL) {
      next->prev = NULL;
    }
  } else {
    next->prev = prev;
    prev->next = next;
  }
  discard(n);
  list->size--;
  //fsck(list);
}

node * new_node(void *value) {
  node *n = malloc(sizeof(node));
  n->next = NULL;
  n->prev = NULL;
  n->value = value;
  return n;
}

list_ref * new_list() {
  list_ref * ret = malloc(sizeof(list_ref));
  ret->head = NULL;
  ret->size = 0;
  return ret;
}

listi list_itr(list_ref *list) {
  listi itr;
  itr.curr  = list->head;
  itr.last = NULL;
  return itr;
}

listi remove_itr(list_ref *list, listi itr, void **value) {
  if (itr.curr == 0) return list_itr(list);
  node *n = itr.curr;
  *value = n->value;
  itr.last = itr.curr->prev;
  itr.curr = itr.curr->next;  
  remove_node(list, n);
  return itr;
}

listi add_itr(list_ref *list, listi itr, void *value) {
  node *nnode = new_node(value);
  add_node_after(list, nnode, itr.curr);
  itr.last = itr.curr;
  itr.curr = nnode;
  return itr;
}

listi next_itr(listi itr) {
  if (itr.curr != 0) {
    itr.last = itr.curr;
    itr.curr = itr.curr->next;
  }
  return itr;
}

bool there(listi itr) {
  return (itr.curr != 0);
}

/**
 * Moves the iterator backwards; even if
 * we have reached the very, very end of the
 * list
 **/
listi prev_itr(listi itr) {
  if (itr.curr != 0) {
    itr.curr = itr.curr->prev;
  } else {
    itr.curr = itr.last;
    itr.last = NULL;
  }
  return itr;
}

void * value_itr(listi itr) {
  if (itr.curr != 0) return itr.curr->value;
  return NULL;
}

void pushv(list_ref *list, void *value) {
  add_node_after(list, new_node(value), list->head);
}

void * popv(list_ref *list) {
  void *ret = NULL;
  if (list->head != NULL) {
    ret = list->head->value;
    remove_node(list, list->head);
  }
  return ret;
}

int sizel(list_ref *list) {
  if (list == NULL) return -1;
  return list->size;
}

void clearl(list_ref *list) {
  while(list->head != NULL) {
    void *value = popv(list);
    if (value != NULL) 
      free(value);
  }
  free(list);
}

