#ifndef INTHEORYNETWORKH
#define INTHEORYNETWORKH
#include "intheory.h"
#include "crc.h"

typedef struct {
  int type;
  int from;
  int to;
  long ticket;
  long slot;
  long value;
  crc_t crc; // must be last, see message_crc fn
} message;

#define CLIENT_VALUE 1
#define PROPOSAL 2
#define ACCEPTED_PROPOSAL 4
#define REJECTED_PROPOSAL 8
#define ACCEPTOR_SET 16
#define SET 32
#define GET 64
#define READ_SUCCESS 128
#define READ_FAILED 256
#define WRITE_SUCCESS 512
#define WRITE_FAILED 1024
#define EXIT 2048

extern int num_nodes;
extern message * (*recv_from)(int, int, long, unsigned int);
extern int (*send_to)(int, long, int, long, long);

char * get_address(int);
int get_port(int);
message * create_message(int, int, long, int, long, long);
int crc_valid(message *);
int send_local(long, int, long, long);
void start_server();
void stop_server();
int send_intheory(int, message *);
void init_network(int, char**, int);
void init_network_nodes(int, char**);
void destroy_network_nodes();
void destroy_network();
#endif
