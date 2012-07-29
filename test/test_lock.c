#include "../src/include/intheory.h"
#include "../src/include/state_machine.h"
#include "../src/include/network.h"
#include "../src/include/store.h"
#include "../src/include/callbacks.h"
#include "include/test_common.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

extern int me;
void test_lock();

int main(int argc, char **args) {
  test_lock();
  return 0;
}


/**
 * Bundle of data used for mocking out
 * events from the store; expired slots, etc.
 */
typedef struct {
  long ms;
  int slot;
  long value;
  unsigned short op;
} test_fire;

void * fire_changed_in(void *data) {
  test_fire *f = (test_fire*) data;
  usleep(f->ms * 1000);
  slot_changed(f->slot, f->value, f->op);
}


/**
 * LOCK SCENARIO: Basic lock, unlock
 **/
long lock_basic_recv[][6] = { 
  { 3, READ_FAILED, -1, 9, -1, 0}
};

long lock_basic_send[][6] = {
  { 3, GET, -1, 9, -1, 0},
  { 3, CLIENT_VALUE, -1, 9, 3, LOCK | TIMEOUT | ASYNC_SEND},
  { 3, CLIENT_VALUE, -1, 9, -1, UNLOCK}
};


void test_lock_basic() {
  sendidx = recvidx = 0;
  me = 3; 
  deadline = 1000; // 1 sec
  recv = lock_basic_recv;
  send = lock_basic_send;

  test_fire f;
  f.slot = 9;
  f.value = 3;
  f.ms = 1000;
  f.op = SLOT_CREATE;

  pthread_t thread;
  pthread_create(&thread, 0, fire_changed_in, &f);

  // init
  init_locks();
  // get lock
  lock(9);
  // maintain
  maintain_locks();
  // release
  unlock(9);
  // cleanup
  destroy_locks();
  pthread_join(thread, 0);
}

/**
 * LOCK SCENARIO: Had to wait for lock
 **/
long lock_late_to_game_recv[][6] = {
  { 3, READ_SUCCESS, -1, 9, 4, 0}
};
long lock_late_to_game_send[][6] = {
  { 3, GET, -1, 9, -1, 0},
  { 3, CLIENT_VALUE, -1, 9, 3, LOCK | TIMEOUT | ASYNC_SEND},
  { 3, CLIENT_VALUE, -1, 9, -1, UNLOCK}
};

void test_lock_late_to_game() {
  sendidx = recvidx = 0;
  me = 3; 
  deadline = 5000; // 5 sec
  recv = lock_late_to_game_recv;
  send = lock_late_to_game_send;

  test_fire other_agent;
  other_agent.slot = 9;
  other_agent.value = 4;
  other_agent.ms = 1000;
  other_agent.op = SLOT_UPDATE;


  test_fire free_agent = other_agent;
  free_agent.value = -1;
  free_agent.ms = 2000;
  free_agent.op = SLOT_DELETE;

  test_fire my_agent = free_agent;
  free_agent.value = 3;
  free_agent.ms = 3000;
  free_agent.op = SLOT_CREATE;

  pthread_t thread, thread2, thread3;
  // init
  init_locks();
  // another agent has the lock for 1 second
  pthread_create(&thread, 0, fire_changed_in, &other_agent);
  // one second later, it is released, firing the cb for that lock
  pthread_create(&thread2, 0, fire_changed_in, &free_agent);
  // one second later, our async send (verified separately) results
  // in a new update with our id in the desired slot
  pthread_create(&thread3, 0, fire_changed_in, &my_agent);

  // get lock
  lock(9);
  // maintain
  maintain_locks();
  // release
  unlock(9);
  // cleanup
  destroy_locks();
  pthread_join(thread, 0);
  pthread_join(thread2, 0);
  pthread_join(thread3, 0);

}

/**
 * LOCK SCENARIO: Get lock immediately, maintain it for two cycles
 **/
long lock_maintained_recv[][6] = {
  { 3, READ_SUCCESS, -1, 9, -1, 0}
};
long lock_maintained_send[][6] = {
  { 3, GET, -1, 9, -1, 0},
  { 3, CLIENT_VALUE, -1, 9, 3, LOCK | TIMEOUT | ASYNC_SEND},
  { 3, CLIENT_VALUE, -1, 9, 3, LOCK | TIMEOUT | ASYNC_SEND},
  { 3, CLIENT_VALUE, -1, 9, -1, UNLOCK}
};

void test_lock_maintained() {
  sendidx = recvidx = 0;
  me = 3; 
  deadline = 1000; // 1 sec
  recv = lock_basic_recv;
  send = lock_basic_send;

  test_fire f;
  f.slot = 9;
  f.value = 3;
  f.ms = 1000;
  f.op = SLOT_CREATE;

  pthread_t thread;
  pthread_create(&thread, 0, fire_changed_in, &f);

  // init
  init_locks();
  // get lock
  lock(9);
  usleep(1000);
  // maintain
  maintain_locks();
  // release
  unlock(9);
  // cleanup
  destroy_locks();
  pthread_join(thread, 0);
}


void test_lock() {  
  set_log_level(NONE);
  _node_count = 4;
  recv_from = &recv_from_scenario; 
  send_to = &send_to_scenario; 
  test_lock_basic();
  test_lock_late_to_game();
  test_lock_maintained();
}
