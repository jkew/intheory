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

// SET Flags
#define ASYNC_SEND 1
#define TIMEOUT 2
#define LOCK 4
#define UNLOCK 8
#define XMANAGE 16

// TODO:
// Optimization could be used to verify certain
// conditions in the local store before proceeding
// with a set LOCK.
// #define FAIL_EARLY 16

enum change_t {
  SLOT_CREATE,
  SLOT_UPDATE,
  SLOT_DELETE
};

enum xslots_t {
  XMANAGE_ENTER_MAINTENANCE_MODE,
  XMANAGE_SET_DEADLINE,
  XMANAGE_CHECKPOINT,
  XMANAGE_PRINT_STATES,
  XMANAGE_FSCK,
  XMANAGE_SHUTDOWN
};

typedef void (*slot_changed_cb)(int slot, long value, unsigned short op);
void start_intheory(int my_index, int node_count, char** allnodes); 
void stop_intheory();
int set_it(int slot, long value, unsigned short flags);
int get_it(int slot, long*retval);
void register_changed_cb(int slot, slot_changed_cb cb);
void unregister_cb(int slot, slot_changed_cb cb);
int my_id();
int num_nodes();
void set_log_level(enum loglevel_t level);
void lock(int slot);
void unlock(int slot);
#endif
