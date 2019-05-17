#include "client.h"
#include "termin.h"

static int server_socket = -1;

/*
 * Connects to the server and reads messages in the while loop,
 * leaves the loop if server closed the connection.
 *
 * Returns -1 if there was an error, 0 otherwise.
 */
int client_connect_to_server(char *address, u_int16_t port, char *username) {
	// Initialize sockaddr structure
	struct sockaddr_in *server_address = create_sockaddr(port);
	if (inet_pton(AF_INET, address, &server_address->sin_addr) <= 0) {
		print_error_message("inet_pton", "invalid server address", ERR_INVALID_ADDRESS);
		return -1;
	}

	// Initialize the socket and connect
	server_socket = create_tcp_socket();
	if (connect(server_socket, (struct sockaddr *)server_address, sizeof(*server_address)) == -1) {
		print_error_message("connect", "could not connect to server", ERR_COULD_NOT_CONNECT_TO_SERVER);
		return -1;
	}

	// Tell the server our username
	write(server_socket, username, C_CHAT_CLIENT_NAME_LENGTH);

	// While there is something to print, do it
	struct client_message msg;
	while(read(server_socket, &msg, sizeof(struct client_message))) {
		if (msg.type == CLIENT_MESSAGE) print_message(msg.sender, msg.body);
		else print_system_message(msg.body);
	}

	// Cleanup after disconnecting
	print_system_message("server closed connection");
	close(server_socket);
	server_socket = -1;

	return 0;
}

/*
 * Sends the given string to the server.
 *
 * Returns -1 if there was an error, 0 otherwise.
 */
int client_send_message(char *message) {
	// Make sure sending a non-empty message
	if (strlen(message) < 1) return 0;

	// Make sure the connection is alive
	if (server_socket == -1) {
		print_system_message("not connected to server");
		return -1;
	}

	// Send the message
	write(server_socket, message, C_CHAT_MESSAGE_LENGTH);

	return 0;
}
