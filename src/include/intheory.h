#ifndef INTHEORYH
#define INTHEORYH

/**
 * InTheory API
 */

enum loglevel_t {
  NONE,
  ERROR,
  GRAPH,
  NOTICE,
  INFO,
  TRACE,
};

typedef void (*slot_changed_cb)(long, long);
void start_intheory(int my_index, int node_count, char** allnodes); 
void stop_intheory();
int set_it(long slot, long value, unsigned short flags);
int get_it(long, long*);
void register_changed_cb(long slot, slot_changed_cb cb);
void unregister_cb(long slot, slot_changed_cb cb);
int my_id();
int num_nodes();
void set_log_level(enum loglevel_t level);

#endif
