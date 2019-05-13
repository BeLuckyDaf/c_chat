#include "server.h"
#include "client.h"

static int is_server = 0;
static int connected = 1;

static void handle_input(char *name) {
	char *buf = (char *)malloc(sizeof(char)*C_CHAT_MESSAGE_LENGTH);
	size_t bufsize = C_CHAT_MESSAGE_LENGTH;
	while(connected) {
		ssize_t bytes_read = getline(&buf, &bufsize, stdin);
		buf[bytes_read-1] = '\0'; // Remove \n at the end
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

static void *connect_to_server(void *data) {
	char **argv = (char **) data;
	client_connect_to_server(argv[3], atoi(argv[4]), argv[2]);
	connected = 0;
}

int main(int argc, char *argv[]) {
	if (argc < 4) return 0;

	is_server = strcmp(argv[1], "+") == 0 ? 1 : 0;
	if (is_server) {
		pthread_t server_tid = start_server(atoi(argv[3]));
		if (server_tid == -1) return -1;
		handle_input(argv[2]);
	} else {
		if (argc < 5) return 0;
		pthread_t tid;
		pthread_create(&tid, NULL, connect_to_server, argv);
		handle_input(argv[2]);
	}

	return 0;
}