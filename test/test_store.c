#include <../src/include/store.h>
#include <../src/include/util.h>
#include <../src/include/intheory.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void test_simple() {
 init_store();
 destroy_store();
}

void test_getset_simple() {
 init_store();
 assert(!exists(0));
 assert(get(0) == 0);
 set(0, 999, -1, 0);
 assert(exists(0));
 assert(get(0) == 999);
 destroy_store();
}

void test_highslot() {
 init_store();
 assert(!exists(777));
 assert(get(777) == 0);
 set(777, 999, -1, 0);
 assert(exists(777));
 assert(get(777) == 999);
 destroy_store();
}

void test_getset_ephem() {
  init_store();
  set(0, 998, get_deadline(750), TIMEOUT);
  assert(exists(0));
  assert(get(0) == 998);
  sleep(1);
  expire_slots();
  assert(!exists(0));
  assert(get(0) == 0);
  destroy_store();
}

void test_lots_of_stuff() {
  init_store();
  int count=1000;
  while(count-- > 0) {
    long t = get_deadline(0);
    int slot = (int) (t % (count + 1));    
    if ((t % 2) == 0) {
      // set perm
      set(slot, t, -1, 0);
      assert(exists(slot));
      assert(get(slot) == t);
    } else {
      // set ephem
      set(slot, t, get_deadline(slot), TIMEOUT);
      assert(exists(slot));
      assert(get(slot) == t);
      usleep(1000*(slot + 1));
      expire_slots();
      assert(!exists(slot));
      assert(get(slot) == 0);
    }
  }
  destroy_store();
}

int main() {
  test_simple();
  test_getset_simple();
  test_getset_ephem();
  test_highslot();
  test_lots_of_stuff();
  return 0;
}
