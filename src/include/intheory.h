#ifndef INTHEORYH
#define INTHEORYH

typedef void (*slot_changed_cb)(long, long);
void start_intheory(char*, int , char**); 
void stop_intheory();
int set_it(long, long);
int get_it(long, long*);
void register_changed_cb(long, slot_changed_cb);
int my_id();

#endif
