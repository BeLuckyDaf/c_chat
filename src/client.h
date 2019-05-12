#ifndef C_CHAT_CLIENT_H
#define C_CHAT_CLIENT_H

#include <zconf.h>
#include <stdio.h>
#include <pthread.h>

#include "utils/alist.h"
#include "utils/sockutil.h"
#include "common.h"

int client_connect_to_server(char *address, u_int16_t port,  char *username);
int client_send_message(char *message);

#endif //C_CHAT_CLIENT_H
