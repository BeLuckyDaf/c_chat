#include "common.h"
#include "termin.h"

/*
 * Converts server_message to client_message structure.
 */
struct client_message server_to_client_message(struct server_message msg) {
	struct client_message result;
	memcpy(result.sender, msg.sender.name, C_CHAT_CLIENT_NAME_LENGTH);
	memcpy(result.body, msg.body, C_CHAT_MESSAGE_LENGTH);
	result.type = CLIENT_MESSAGE;
	return result;
}
