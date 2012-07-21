#include <../src/include/list.h>
#include <../src/include/util.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

list_ref *list = NULL;

void test_clear() {
  int *val = malloc(sizeof(int));
  *val = 999;
  list = new_list();
  pushv(list, val);
  assert(1 == sizel(list));
  assert(*val == 999);
  clearl(list);
}


void test_push_pop() {
  int *val = malloc(sizeof(int));
  *val = 999;
  list = new_list();
  pushv(list, val);
  pushv(list, val);
  pushv(list, val);
  assert(3 == sizel(list));
  assert(*val == 999);
  assert(val == popv(list));
  assert(2 == sizel(list));
  assert(val == popv(list));
  assert(1 == sizel(list));
  assert(val == popv(list));
  assert(0 == sizel(list));
  clearl(list);
  free(val);
}


void test_itr() {
  int input[] = { 0, 1, 2, 999, 3 };
  int i;
  list = new_list();
  listi itr = list_itr(list);
  for (i = 0; i < 5; i++) {
    itr = add_itr(list, itr, &input[i]);
    assert((i + 1) == sizel(list));
    assert(input[i] == *((int *)value_itr(itr)));
  }
  assert(5 == sizel(list));
  itr = list_itr(list);
  i = 0;
  while(there(itr)) {
    assert(input[i] == *((int *)value_itr(itr)));
    i++;
    itr = next_itr(itr);
  }
  
  itr = prev_itr(itr); // back to the end of the list

  i = 4;
  while(there(itr)) {
    assert(input[i] == *((int *)value_itr(itr)));
    i--;
    itr = prev_itr(itr);  
  }

  itr = list_itr(list);
  int *val;
  while(there(itr = remove_itr(list, itr, &val))) {
    // no need to free val
  }
  assert(0 == sizel(list));
  clearl(list);
}

int main() {
 test_clear();
 test_itr();
 return 0;
}
