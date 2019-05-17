#include "server.h"
#include "termin.h"

static p_array_list clients = NULL;
static int server_socket = -1;

// Need it as a constant variable because will need to copy it
static const char server_name[C_CHAT_CLIENT_NAME_LENGTH] = "server";

pthread_t start_server(u_int16_t port) {
	// Initializing clients list
	if (clients != NULL) {
		print_system_message("error", "the server has already started");
		return -1;
	}
	clients = create_array_list(2);

	// Initializing server socket
	server_socket = create_tcp_socket();
	struct sockaddr_in *server_address = create_sockaddr(port);
	if (bind(server_socket, (struct sockaddr*)server_address, sizeof(*server_address)) == -1) {
		print_error_message("bind", "bind failed", errno);
		return -1;
	}

	pthread_t tid;
	pthread_create(&tid, NULL, connection_handler, NULL);

	print_system_message("server", "started");

	return tid;
}

void *connection_handler(void* data) {
	// Start listening
	listen(server_socket, 3);

	// Client address and its size
	struct sockaddr_in cl_addr = {0};
	socklen_t cl_addr_size = sizeof(cl_addr);

	while(server_socket > 0) {
		// Accept the connection and ignore if failed
		int client_socket = accept(server_socket, (struct sockaddr*)&cl_addr, &cl_addr_size);
		if (client_socket == -1) {
			print_error_message("accept", "accept failed", errno);
			continue;
		}

		// Receive client's greeting server_message
		struct greeting_message message = {0};
		read(client_socket, &message, sizeof(struct greeting_message));

		// Adding new client to the list of clients
		struct client *new_client = create_client(client_socket, message.username, &cl_addr);
		if (add_client(*new_client) == -1) {
			print_client_exists_message(new_client->name);
			close(client_socket);
			continue;
		}

		pthread_t client_thread_id;
		pthread_create(&client_thread_id, NULL, client_handler, new_client);
	}

	return NULL;
}

int add_client(struct client newcl) {
	int it = array_list_iter(clients);
	struct client *cl;

	// Checking if such username is occupied already
	while(it >= 0) {
		cl = (struct client *) array_list_get(clients, it);
		if (strcmp(cl->name, newcl.name) == 0) return -1;
		it = array_list_next(clients, it);
	}

	// If not, copying to another structure and adding to the list
	cl = (struct client *) malloc(sizeof(struct client));
	memcpy(cl, &newcl, sizeof(struct client));

	array_list_add(clients, cl);

	return 0;
}

struct client *create_client(int sockfd, char *name, struct sockaddr_in *addr) {
	// Packing up all the data into a structure instance
	struct client *result = (struct client*) malloc(sizeof(struct client));
	memcpy(result->name, name, C_CHAT_CLIENT_NAME_LENGTH);
	memcpy(&result->address, addr, sizeof(struct sockaddr_in));
	result->sockfd = sockfd;

	return result;
}

void *client_handler(void *data) {
	// Initializing data
	struct client *current_client = (struct client *) data;
	char current_message[C_CHAT_MESSAGE_LENGTH] = {0};
	struct client_message current_client_message;
	char connect_message[C_CHAT_MESSAGE_LENGTH] = {0};

	// Tell all the clients that a new one has connected
	sprintf(connect_message, "=== %s connected ===", current_client->name);
	broadcast_server_message(connect_message);

	// Read clients messages and broadcast them to everyone
	while(read(current_client->sockfd, current_message, C_CHAT_MESSAGE_LENGTH) > 0) {
		memcpy(current_client_message.sender, current_client->name, C_CHAT_CLIENT_NAME_LENGTH);
		memcpy(current_client_message.body, current_message, C_CHAT_MESSAGE_LENGTH);
		broadcast_client_message(current_client_message);

		memset(current_message, 0, C_CHAT_MESSAGE_LENGTH);
	}

	// Remove the client from the list of clients now, to free the client
	// name for further connections
	if (remove_client(current_client->name) != 0) {
		print_system_message("remove_client", "could not remove the client from list");
	}

	// Close the socket since the client is now officially disconnected
	close(current_client->sockfd);

	char disconnect_message[C_CHAT_MESSAGE_LENGTH] = {0};
	sprintf(disconnect_message, "=== %s disconnected ===", current_client->name);
	broadcast_server_message(disconnect_message);

	return NULL;
}

int remove_client(char *name) {
	int it = array_list_iter(clients);
	struct client *cl;

	// Trying to find the client in the list
	while(it >= 0) {
		cl = (struct client *) array_list_get(clients, it);
		if (strcmp(cl->name, name) == 0) {
			array_list_remove_at(clients, it);
			free(cl);
			return 0;
		}
		it = array_list_next(clients, it);
	}

	return -1;
}

int broadcast_client_message(struct client_message msg) {
	int it = array_list_iter(clients);
	struct client *cl;

	print_message(msg.sender, msg.body);

	// Sending message to every client on the list
	while(it >= 0) {
		cl = (struct client *) array_list_get(clients, it);
		write(cl->sockfd, &msg, sizeof(struct client_message));
		it = array_list_next(clients, it);
	}

	return 0;
}

int broadcast_server_message(char *msg) {
	int it = array_list_iter(clients);
	struct client *cl;
	struct client_message clmsg;
	memcpy(clmsg.sender, server_name, C_CHAT_CLIENT_NAME_LENGTH);
	memcpy(clmsg.body, msg, C_CHAT_MESSAGE_LENGTH);

	print_message(clmsg.sender, clmsg.body);

	// Sending message to every client on the list
	while(it >= 0) {
		cl = (struct client *) array_list_get(clients, it);
		write(cl->sockfd, &clmsg, sizeof(struct client_message));
		it = array_list_next(clients, it);
	}

	return 0;
}
