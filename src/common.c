#include "common.h"

void print_client_message(struct client_message msg) {
	printf("<%s>: %s\n", msg.sender, msg.body);
}

struct client_message server_to_client_message(struct server_message msg) {
	struct client_message result;
	memcpy(result.sender, msg.sender.name, C_CHAT_CLIENT_NAME_LENGTH);
	memcpy(result.body, msg.body, C_CHAT_MESSAGE_LENGTH);
	return result;
}
