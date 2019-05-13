#include "server.h"
#include "client.h"

static int is_server = 0;
static int connected = 1;

void handle_input(char *name) {
	char *buf = (char *)malloc(sizeof(char)*C_CHAT_MESSAGE_LENGTH);
	size_t bufsize = C_CHAT_MESSAGE_LENGTH;
	while(connected) {
		getline(&buf, &bufsize, stdin);
		if (is_server) {
			struct client_message msg;
			strcpy(msg.sender, name);
			strcpy(msg.body, buf);
			broadcast_client_message(msg);
		} else {
			client_send_message(buf);
		}
	}
}

void *connect_to_server(void *data) {
	char **argv = (char **) data;
	client_connect_to_server(argv[3], atoi(argv[4]), argv[2]);
	connected = 0;
}

int main(int argc, char *argv[]) {
	if (argc < 4) return 0;

	is_server = strcmp(argv[1], "+") == 0 ? 1 : 0;
	if (is_server) {
		pthread_t server_tid = start_server(atoi(argv[3]));
		handle_input(argv[2]);
	} else {
		if (argc < 5) return 0;
		pthread_t tid;
		pthread_create(&tid, NULL, connect_to_server, argv);
		handle_input(argv[2]);
	}

	return 0;
}