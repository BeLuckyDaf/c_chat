#ifndef C_CHAT_COMMON_H
#define C_CHAT_COMMON_H

#include <sys/types.h>
#include <netinet/in.h>

#define C_CHAT_CLIENT_NAME_LENGTH 256
#define C_CHAT_MESSAGE_LENGTH 1024

struct client {
	int sockfd;
	struct sockaddr_in address;
	char name[C_CHAT_CLIENT_NAME_LENGTH];
};

struct message {
	struct client sender;
	char body[C_CHAT_MESSAGE_LENGTH];
};

struct greeting_message {
	char username[C_CHAT_CLIENT_NAME_LENGTH];
};

#endif // C_CHAT_COMMON_H