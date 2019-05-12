#include "server.h"

static p_array_list clients = NULL;
static int server_socket = -1;

pthread_t start_server(u_int16_t port) {
	// Initializing clients list
	if (clients != NULL) {
		printf("<error>: the server has already started.\n");
		return -1;
	}
	clients = create_array_list(2);

	// Initializing server socket
	server_socket = create_tcp_socket();
	struct sockaddr_in *server_address = create_sockaddr(port);
	if (bind(server_socket, (struct sockaddr*)server_address, sizeof(*server_address)) == -1) {
		printf("<bind>: bind failed, error code %d.\n", errno);
		return -1;
	}

	pthread_t tid;
	pthread_create(&tid, NULL, connection_handler, NULL);

	printf("<server>: started.\n");

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
			printf("<accept>: accept failed, error code %d.\n", errno);
			continue;
		}

		// Receive client's greeting message
		struct greeting_message message = {0};
		read(client_socket, &message, sizeof(struct greeting_message));

		// Adding new client to the list of clients
		struct client *new_client = create_client(client_socket, message.username, &cl_addr);
		if (add_client(*new_client) == -1) {
			printf("<accept>: client \"%s\" already exists.\n", new_client->name);
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

	cl = (struct client *) malloc(sizeof(struct client));
	memcpy(cl, &newcl, sizeof(struct client));

	array_list_add(clients, cl);

	return 0;
}

struct client *create_client(int sockfd, char *name, struct sockaddr_in *addr) {
	struct client *result = (struct client*) malloc(sizeof(struct client));
	memcpy(result->name, name, C_CHAT_CLIENT_NAME_LENGTH);
	memcpy(&result->address, addr, sizeof(struct sockaddr_in));
	result->sockfd = sockfd;

	return result;
}

void *client_handler(void *data) {
	struct client *current_client = (struct client *) data;
	char message[C_CHAT_MESSAGE_LENGTH] = {0};
	struct message msg;

	printf("<server>: %s connected.\n", current_client->name);

	while(read(current_client->sockfd, message, C_CHAT_MESSAGE_LENGTH) > 0) {
		// Send to everyone here
		printf("<%s>: %s\n", current_client->name, message);

		msg.sender = *current_client;
		memcpy(msg.body, message, C_CHAT_MESSAGE_LENGTH);
		send_to_all_clients(msg);

		memset(message, 0, C_CHAT_MESSAGE_LENGTH);
	}

	// Removing the client from the list of clients now, to free the client
	// name for further connections
	if (remove_client(current_client->name) != 0) {
		printf("<remove_client>: could not remove the client from list.\n");
	}

	// Closing the socket since the client is now officially disconnected
	close(current_client->sockfd);

	printf("<server>: %s disconnected.\n", current_client->name);

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
			return 0;
		}
		it = array_list_next(clients, it);
	}

	return -1;
}

int send_to_all_clients(struct message msg) {
	int it = array_list_iter(clients);
	struct client *cl;

	// Sending message to every client on the list
	while(it >= 0) {
		cl = (struct client *) array_list_get(clients, it);
		write(cl->sockfd, &msg, sizeof(struct message));
		it = array_list_next(clients, it);
	}

	return 0;
}
