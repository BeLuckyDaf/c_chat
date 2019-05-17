#include "server.h"
#include "client.h"
#include "termin.h"

static int is_server = 0;
static int connected = 1;

static void handle_input(char *name) {
	char *buf = (char *)malloc(sizeof(char)*C_CHAT_MESSAGE_LENGTH);
	size_t bufsize = C_CHAT_MESSAGE_LENGTH;

	enable_chat_mode();

	while(connected) {
		ssize_t bytes_read = read_line(buf, bufsize, "> ");
		if (bytes_read <= 1) continue;
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
	struct connect_data *conndata = (struct connect_data*) data;
	client_connect_to_server(conndata->address, conndata->port, conndata->username);
	connected = 0;
}

int main(int argc, char *argv[]) {
	char buf[1024];
	char *username = (char*) malloc(C_CHAT_CLIENT_NAME_LENGTH);
	char *server_address = (char*) malloc(INET_ADDRSTRLEN);
	u_int16_t port;

	printf("Username: ");
	scanf("%s", username);

	printf("Are you a server? (Y/N): ");
	scanf("%s", buf);
	is_server = buf[0] == 'y' || buf[0] == 'Y' ? 1 : 0;

	if (!is_server) {
		printf("Server address: ");
		scanf("%s", buf);
		strcpy(server_address, buf);
	}

	printf("Server port: ");
	scanf("%hu", &port);

	if (is_server) {
		pthread_t server_tid = start_server(port);
		if (server_tid == -1) return -1;
		handle_input(username);
	} else {
		pthread_t tid;
		struct connect_data *conndata = malloc(sizeof(struct connect_data));
		conndata->username = username;
		conndata->address = server_address;
		conndata->port = port;
		pthread_create(&tid, NULL, connect_to_server, conndata);
		handle_input(username);
	}

	return 0;
}