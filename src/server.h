#ifndef C_CHAT_SERVER_H
#define C_CHAT_SERVER_H

#include <zconf.h>
#include <stdio.h>
#include <pthread.h>

#include "utils/alist.h"
#include "utils/sockutil.h"
#include "common.h"

pthread_t start_server(u_int16_t port);
void *connection_handler(void *data);
void *client_handler(void *data);

int broadcast_client_message(struct client_message msg);
int broadcast_server_message(char *msg);

int add_client(struct client newcl);
int remove_client(char *name);
struct client *create_client(int sockfd, char *name, struct sockaddr_in *addr);

#endif //C_CHAT_SERVER_H
