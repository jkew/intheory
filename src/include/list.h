#ifndef ITLISTH
#define ITLISTH
#include "util.h"
#include "list.h"

typedef struct {
  void *value;
  struct node *prev;
  struct node *next;
} node;

typedef struct {
  node *head;
  int size;
} list_ref;

typedef struct {
  node *curr;
  node *last;
} listi;

list_ref * new_list();
listi list_itr(list_ref *list);
bool there(listi itr);
listi next_itr(listi itr);
listi prev_itr(listi itr);
listi remove_itr(list_ref *list, listi itr, void **value);
listi add_itr(list_ref *list, listi itr, void *value);
void * value_itr(listi itr);
void clearl(list_ref *list);
void pushv(list_ref *list, void *value);
void * popv(list_ref *list);
int sizel(list_ref *list);

#endif
