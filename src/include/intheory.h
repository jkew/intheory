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

// Value Flags
#define ASYNC_SEND 1
#define TIMEOUT 2
#define FAIL_EARLY 4
#define LOCK 8
#define UNLOCK 16

typedef void (*slot_changed_cb)(int slot, long value);
void start_intheory(int my_index, int node_count, char** allnodes); 
void stop_intheory();
int set_it(int slot, long value, unsigned short flags);
int get_it(int slot, long*retval);
void register_changed_cb(int slot, slot_changed_cb cb);
void unregister_cb(int slot, slot_changed_cb cb);
int my_id();
int num_nodes();
void set_log_level(enum loglevel_t level);

#endif
