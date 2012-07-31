#ifndef INTHEORYNETWORKH
#define INTHEORYNETWORKH
#include "intheory.h"
#include "crc.h"

typedef struct {
  unsigned short type;
  unsigned short from;
  unsigned short to;
  long ticket;
  unsigned short flags;
  int slot;
  long value;
  unsigned long deadline;
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

extern message * (*recv_from)(int, int, long, unsigned int);
extern int (*send_to)(int, long, int, long, long, unsigned short);
extern int intheory_remote;

char * get_address(int node);
int get_port(int node);
message * create_message(unsigned short from, 
			 unsigned short to, 
			 long ticket, 
			 short type, 
			 int slot, 
			 long value, 
			 unsigned short flags);
int crc_valid(message *msg);
void start_server();
void stop_server();
int send_intheory(int, message *);
void init_network(int, int, char**, int);
void init_network_nodes(int, int, char**);
void destroy_network_nodes();
void destroy_network();

struct sockaddr_un it_domain_sockaddr(int id, int delete);
int it_domain_socket();
int it_remote_socket();
void set_it_socket_opts(int socketfd);
#endif
