#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include "include/intheory.h"
#include "include/logger.h"
#include "include/callbacks.h"

typedef struct {
  long slot;
  long value;
  int runstate;
  slot_changed_cb cb;
  pthread_t thread;
} callback;

callback *cbs = 0;
int maxcbs = 0;

void cb_worker(void *callback_struct) {
  callback *cbs = callback_struct; 
  cbs->cb(cbs->slot, cbs->value);
  cbs->runstate = 1; 
}

void slot_changed(long slot, long value) {
  int i;
  for(i = 0; i < maxcbs; i++) {
    if (cbs[i].slot == slot && cbs[i].runstate == 0) {
      cbs[i].value = value;
      pthread_create(&(cbs[i].thread), 0, cb_worker, &cbs[i]);
      cbs[i].runstate = 2;
    } else {
      if (cbs[i].runstate == 1) {
	pthread_join(&cbs[i].thread, 0);
	cbs[i].thread = 0;
	cbs[i].runstate = 0;
      }
    }
  }
}

void register_changed_cb(long slot, slot_changed_cb cb) {
  int newsize = maxcbs + 1;
  callback *tmp = malloc(sizeof(callback)*newsize);
  memcpy(tmp, cbs, maxcbs);
  callback *old = cbs;
  cbs = tmp;
  discard(old);
  cbs[maxcbs].slot = slot;
  cbs[maxcbs].cb = cb;
  cbs[maxcbs].runstate = 0;
  cbs[maxcbs].thread = 0;
  maxcbs = newsize;
}

void unregister_cb(long slot, slot_changed_cb cb) {
  int i;
  for(i = 0; i < maxcbs; i++) {
    if (cbs[i].slot == slot && cbs[i].cb == cb) {
      cbs[i].slot = -1;
      return;
    }
  }
}

void destroy_cb() {
  int i;
  for(i = 0; i < maxcbs; i++) {
    if (cbs[i].runstate == 1) {
      cbs[i].runstate = 0;
      pthread_join(&cbs[i].thread, 0);
    }
  }
  discard(cbs);
}
