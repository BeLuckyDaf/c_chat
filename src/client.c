#include "client.h"

static int server_socket = -1;

int client_connect_to_server(char *address, u_int16_t port, char *username) {
	struct sockaddr_in *server_address = create_sockaddr(port);
	if (inet_pton(AF_INET, address, &server_address->sin_addr) <= 0) {
		printf("<client>: invalid server address.\n");
		return -1;
	}

	server_socket = create_tcp_socket();
	if (connect(server_socket, (struct sockaddr *)server_address, sizeof(*server_address)) == -1) {
		printf("<client>: could not connect to server.\n");
		return -1;
	}

	write(server_socket, username, C_CHAT_CLIENT_NAME_LENGTH);

	struct client_message msg;
	while(read(server_socket, &msg, sizeof(struct client_message))) {
		printf("<%s>: %s\n", msg.sender, msg.body);
	}

	close(server_socket);

	return 0;
}

int client_send_message(char *message) {
	write(server_socket, message, C_CHAT_MESSAGE_LENGTH);
	return 0;
}
