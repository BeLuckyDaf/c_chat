#include <server.h>

int main() {
	pthread_t server_tid = start_server(12000);
	pthread_join(server_tid, NULL);

	return 0;
}