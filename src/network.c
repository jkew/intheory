#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include "include/intheory.h"
#include "include/state_machine.h"
#include "include/network.h"
#define INITIAL_RING_SIZE 64

message **input_ring;
int ring_size = -1;
int write_ipos;
int role_read_ipos[4];

pthread_mutex_t write_lock;

message * (*recv_from)(int, int, long, unsigned int) = 0;
int (*send_to)(int, long, int, long, long) = 0;

int advance_writer() {
  int next_pos = (write_ipos + 1) % ring_size;
  // look for the next writable spot
  while(input_ring[next_pos] != 0) {
    next_pos = (next_pos + 1) % ring_size;
  }
  write_ipos = next_pos;
  return next_pos;
}

int advance_role(int r) {
  int next_pos = (role_read_ipos[r] + 1) % ring_size;
  role_read_ipos[r] = next_pos;
  return next_pos;
}

message * get_if_matches(int i, int from_node, long slot, unsigned int mask) {
  message *mesg = input_ring[i];
  if (mesg == 0) return 0;
  if (from_node != -1 && (mesg->from) != from_node) return 0;
  if (slot != -1 && mesg->slot != slot) return 0;
  if (! (mesg->type & mask)) return 0;
  input_ring[i] = 0;
  return mesg;
}

void add_message(message *msg) {
  pthread_mutex_lock(&write_lock);
  // log_message("add_message", msg);
  input_ring[write_ipos] = msg;
  advance_writer();
  pthread_mutex_unlock(&write_lock);
}

crc_t message_crc(message *msg) {
  crc_t c = crc_init();
  // crc everything except the last crc field
  c = crc_update(c, (void *)msg, ((void *)&(msg->crc)) - ((void*)msg));
  return crc_finalize(c);
}

int crc_valid(message *msg) {
  crc_t c = message_crc(msg);
  return c == msg->crc;
}

message * create_message(int from, int to, long ticket, int type, long slot, long value) {
  message *msg; 
  msg = malloc(sizeof(message));
  msg->from = from;
  msg->to = to;
  msg->type = type;
  msg->ticket = ticket;
  msg->slot = slot;
  msg->value = value;
  crc_t c = message_crc(msg);
  msg->crc = c;
  //  log_message("create_message", msg);
  return msg;
}

message * __recv_from(int r, int from_node, long slot, unsigned int mask) {
  int pos = role_read_ipos[r];
  if (pos < 0) {
    pos = role_read_ipos[r] = 0;
  }
  int initial_pos = pos;
  int rounds = num_nodes;
  message * msg = get_if_matches(pos, from_node, slot, mask);
  while(msg == 0) {
    pos = advance_role(r);
    // If we loop around the message ring rounds times, we timeout and
    // return nothing.
    if (pos == initial_pos) {      
      if (rounds <= 0) {
	return 0;
      }
      usleep(100000);
      rounds--;
    }
    msg = get_if_matches(pos, from_node, slot, mask);
  }
  log_message("recv_from", msg);
  return msg;
}

int __send_local(long ticket, int type, long slot, long value) {
  message *msg = create_message(my_id(), my_id(), ticket, type, slot, value);
  log_message("send_local", msg);
  add_message(msg);
  return 1;
}

int __send_to(int node, long ticket, int type, long slot, long value) {
  int n = node % num_nodes;
  if (n == my_id()) 
    return __send_local(ticket, type, slot, value);
  return send_intheory(node, create_message(my_id(), node, ticket, type, slot, value));
}


void init_network(int _num_nodes, char *_nodes[], int _ring_size) {
  pthread_mutex_init(&write_lock, 0);
  init_network_nodes(_num_nodes, _nodes);
  recv_from = __recv_from;
  send_to = __send_to;
  ring_size = _ring_size;
  write_ipos = 0;
  role_read_ipos[PROPOSER] = -1;
  role_read_ipos[ACCEPTOR] = -1;
  role_read_ipos[CLIENT] = -1;
  role_read_ipos[LEARNER] = -1;
  input_ring = malloc(sizeof(message *)*ring_size);
  memset(input_ring, 0, sizeof(message *)*ring_size);
}

void destroy_network() {
  destroy_network_nodes();
  int i;
  for (i = 0; i < ring_size; i++)
    if (input_ring[i] != 0)
      free(input_ring[i]);
  free(input_ring);
  input_ring = 0;
  ring_size = 0;
  pthread_mutex_lock(&write_lock);
}

