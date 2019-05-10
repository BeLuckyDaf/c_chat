#ifndef C_CHAT_COMMON_H
#define C_CHAT_COMMON_H

#include <sys/socket.h>
#include <sys/types.h>

struct client {
	struct sockaddr_in address;
	char name[256];
};

struct message {
	struct client *sender;
	char body[1024];
};

#endif // C_CHAT_COMMON_H