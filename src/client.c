#include "client.h"
#include "termin.h"

static int server_socket = -1;

int client_connect_to_server(char *address, u_int16_t port, char *username) {
	// Initialize sockaddr structure
	struct sockaddr_in *server_address = create_sockaddr(port);
	if (inet_pton(AF_INET, address, &server_address->sin_addr) <= 0) {
		print_system_message("client", "invalid server address");
		return -1;
	}

	// Initialize the socket and connect
	server_socket = create_tcp_socket();
	if (connect(server_socket, (struct sockaddr *)server_address, sizeof(*server_address)) == -1) {
		print_system_message("client", "could not connect to server");
		return -1;
	}

	// Tell the server our username
	write(server_socket, username, C_CHAT_CLIENT_NAME_LENGTH);

	// While there is something to print, do it
	struct client_message msg;
	while(read(server_socket, &msg, sizeof(struct client_message))) {
		print_message(msg.sender, msg.body);
	}

	// Cleanup after disconnecting
	print_system_message("client", "server closed connection");
	close(server_socket);
	server_socket = -1;

	return 0;
}

int client_send_message(char *message) {
	// Make sure sending a non-empty message
	if (strlen(message) < 1) return 0;

	// Make sure the connection is alive
	if (server_socket == -1) {
		print_system_message("client", "not connected to server");
		return -1;
	}

	// Send the message
	write(server_socket, message, C_CHAT_MESSAGE_LENGTH);

	return 0;
}
