#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "include/intheory.h"
#include "include/network.h"
#define INITIAL_RING_SIZE 64

char ** nodes = 0; 
int num_nodes = -1;
int me = -1;

int my_id() {
  return me;
}

char * to_addr(char *nodePort) {
  char buffer[256];
  int i = 0;
  while(nodePort[i] != ':' && nodePort[i] != 0 && i < 256) {
    buffer[i] = nodePort[i];
    i++;
  }
  buffer[i] = 0;
  int size = i + 1;
  char *node = malloc(sizeof(char)*size);
  strncpy(node, buffer, size);
  return node;
}

int to_port(char *nodePort) {
  char buffer[256];
  int i = 0;
  while(nodePort[i] != ':' && nodePort[i] != 0 && i < 256)
    i++;
  i++;
  return atoi(&nodePort[i]);
}

int cmp_fn(const void *one, const void *two) {
  const char *one_ = *(const char **)one;
  const char *two_ = *(const char **)two;
  return strcmp(one_, two_);
}

char * get_address(int node_num) {
  assert(node_num >= 0);
  int n = node_num % num_nodes;
  return to_addr(nodes[n]);
}

int get_port(int node_num) {
  assert(node_num >= 0);
  int n = node_num % num_nodes;
  return to_port(nodes[n]);
}

void init_network_nodes(int _num_nodes, char *_nodes[]) {
  char *my_nodes = _nodes[0];
  num_nodes = _num_nodes;
  nodes = malloc(sizeof(char *)*_num_nodes);
  int i;
  for (i = 0; i < num_nodes; i++) {
    nodes[i] = (char *) malloc(sizeof(char)*256);
    strncpy(nodes[i], _nodes[i], 255 );
  }
  qsort(nodes, num_nodes, sizeof(char *), cmp_fn);
  for (i = 0; i < num_nodes; i++) {
    if (strncmp(my_nodes, nodes[i], 256) == 0) {
      me = i;
      break;
    }
  }
}

void destroy_network_nodes() {
  int i;
  for (i = 0; i < num_nodes; i++)
    discard(nodes[i]);
  discard(nodes);
  nodes = 0;
  num_nodes = 0;
}

