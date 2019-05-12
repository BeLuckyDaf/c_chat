#include "server.h"
#include "client.h"

int main(int argc, char *argv[]) {
	if (argc < 4) return 0;

	if (strcmp(argv[1], "+") == 0) {
		pthread_t server_tid = start_server(atoi(argv[3]));
		pthread_join(server_tid, NULL);
	} else {
		if (argc < 5) return 0;
		client_connect_to_server(argv[3], atoi(argv[4]), argv[2]);
	}

	return 0;
}