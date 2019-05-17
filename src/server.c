#include "server.h"
#include "termin.h"

static p_array_list clients = NULL;
static int server_socket = -1;
static pthread_mutex_t clients_mutex;

// Need it as a constant variable because will need to copy it
static const char server_name[C_CHAT_CLIENT_NAME_LENGTH] = "server";

/*
 * Initializes and starts the server.
 */
pthread_t start_server(u_int16_t port) {
	pthread_mutex_init(&clients_mutex, NULL);

	// Initializing clients list
	if (clients != NULL) {
		print_error_message("error", "the server has already started", ERR_SERVER_ALREADY_STARTED);
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

	print_system_message("server started");

	return tid;
}

/*
 * Handles incoming connections.
 *
 * Accepts new cients and adds the to the list of all clients,
 * if such client already exists, closes the connection.
 *
 * Then creates a new thread for that client, using client_handler.
 */
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

		LOCK(clients_mutex);
		int add_client_result = add_client(*new_client);
		UNLOCK(clients_mutex);
		if (add_client_result == -1) {
			print_client_exists_message(new_client->name);
			close(client_socket);
			continue;
		}

		pthread_t client_thread_id;
		pthread_create(&client_thread_id, NULL, client_handler, new_client);
	}

	return NULL;
}

/*
 * Checks if specified client exists in the list, if not, adds
 * the new client and returns 0, otherwise returns -1.
 */
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

/*
 * Allocates memory for an instance of the client structure and fills it
 * using the given information about the client.
 */
struct client *create_client(int sockfd, char *name, struct sockaddr_in *addr) {
	// Packing up all the data into a structure instance
	struct client *result = (struct client*) malloc(sizeof(struct client));
	memcpy(result->name, name, C_CHAT_CLIENT_NAME_LENGTH);
	memcpy(&result->address, addr, sizeof(struct sockaddr_in));
	result->sockfd = sockfd;

	return result;
}

/*
 * Reads messages from the client while they are alive.
 *
 * Whenever client closes their connection, removes them from
 * the list of clients.
 */
void *client_handler(void *data) {
	// Initializing data
	struct client *current_client = (struct client *) data;
	char current_message[C_CHAT_MESSAGE_LENGTH] = {0};
	struct client_message current_client_message;
	char connect_message[C_CHAT_MESSAGE_LENGTH] = {0};

	// Tell all the clients that a new one has connected
	sprintf(connect_message, "%s connected", current_client->name);
	broadcast_system_message(connect_message);

	// Read clients messages and broadcast them to everyone
	while(read(current_client->sockfd, current_message, C_CHAT_MESSAGE_LENGTH) > 0) {
		memcpy(current_client_message.sender, current_client->name, C_CHAT_CLIENT_NAME_LENGTH);
		memcpy(current_client_message.body, current_message, C_CHAT_MESSAGE_LENGTH);
		current_client_message.type = CLIENT_MESSAGE;
		broadcast_client_message(current_client_message);

		memset(current_message, 0, C_CHAT_MESSAGE_LENGTH);
	}

	// Remove the client from the list of clients now, to free the client
	// name for further connections
	LOCK(clients_mutex);
	int remove_client_result = remove_client(current_client->name);
	UNLOCK(clients_mutex);
	if (remove_client_result != 0) {
		print_error_message("alist", "could not remove the client from list", ERR_ALIST_COULD_NOT_REMOVE_CLIENT);
	}

	// Close the socket since the client is now officially disconnected
	close(current_client->sockfd);

	char disconnect_message[C_CHAT_MESSAGE_LENGTH] = {0};
	sprintf(disconnect_message, "%s disconnected", current_client->name);
	broadcast_system_message(disconnect_message);

	return NULL;
}

/*
 * Removes the specified client from the list and returns 0,
 * if the client was not on the list returns -1.
 */
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

/*
 * Sends the given client message to all clients on the list.
 */
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

/*
 * Sends the given system message to all clients on the list.
 */
int broadcast_system_message(char *msg) {
	int it = array_list_iter(clients);
	struct client *cl;
	struct client_message clmsg;
	memcpy(clmsg.sender, server_name, C_CHAT_CLIENT_NAME_LENGTH);
	memcpy(clmsg.body, msg, C_CHAT_MESSAGE_LENGTH);
	clmsg.type = SYSTEM_MESSAGE;

	print_system_message(clmsg.body);

	// Sending message to every client on the list
	while(it >= 0) {
		cl = (struct client *) array_list_get(clients, it);
		write(cl->sockfd, &clmsg, sizeof(struct client_message));
		it = array_list_next(clients, it);
	}

	return 0;
}
