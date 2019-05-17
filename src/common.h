#ifndef C_CHAT_COMMON_H
#define C_CHAT_COMMON_H

#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define LOCK(M) (pthread_mutex_lock(&(M)))
#define UNLOCK(M) (pthread_mutex_unlock(&(M)))

#define C_CHAT_CLIENT_NAME_LENGTH 256
#define C_CHAT_MESSAGE_LENGTH 1024

#define ERR_SERVER_ALREADY_STARTED 1
#define ERR_ALIST_COULD_NOT_REMOVE_CLIENT 2
#define ERR_INVALID_ADDRESS 3
#define ERR_COULD_NOT_CONNECT_TO_SERVER 4

enum MESSAGE_TYPE {
    SYSTEM_MESSAGE,
    CLIENT_MESSAGE
};

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
    int type;
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

struct client_message server_to_client_message(struct server_message msg);

#endif // C_CHAT_COMMON_H