#ifndef C_CHAT_COMMON_H
#define C_CHAT_COMMON_H

#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

#define C_CHAT_CLIENT_NAME_LENGTH 256
#define C_CHAT_MESSAGE_LENGTH 1024

struct client {
	int sockfd;
	struct sockaddr_in address;
	char name[C_CHAT_CLIENT_NAME_LENGTH];
};

struct server_message {
	struct client sender;
	char body[C_CHAT_MESSAGE_LENGTH];
};

struct client_message {
    char sender[C_CHAT_CLIENT_NAME_LENGTH];
    char body[C_CHAT_MESSAGE_LENGTH];
};

struct greeting_message {
	char username[C_CHAT_CLIENT_NAME_LENGTH];
};

struct connect_data {
    char *address;
    u_int16_t port;
    char *username;
};

void print_client_message(struct client_message);
struct client_message server_to_client_message(struct server_message msg);

#endif // C_CHAT_COMMON_H