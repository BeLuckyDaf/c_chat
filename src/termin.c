#include "termin.h"
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

static struct termios defterm;

void enable_chat_mode() {
	struct termios cm;
	tcgetattr(STDIN_FILENO, &cm);
	defterm = cm;
	cm.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &cm);

	setvbuf(stdout, NULL, _IONBF, 1024);

	atexit(reset_chat_mode);
}

void reset_chat_mode() {
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &defterm);
}

int read_line(char *buf, size_t bufsize, char *prefix) {
	int cur = 0;
	char c;
	memset(buf, 0, bufsize);

	printf("\r\033[K%s", prefix);

	int skip = 0;
	while (read(STDIN_FILENO, &c, 1)) {
		if (skip > 0) {
			skip--;
			continue;
		}

		if (c == 127 && cur > 0) {
			buf[--cur] = '\0';
		}
		else if (c == '\n') {
			buf[cur] = c;
			buf[++cur] = '\0';
			printf("\r\033[K");
			return cur;
		}
		else if (iscntrl(c)) {
			skip = 2;
			continue;
		}
		else if (cur < bufsize - 2) {
			buf[cur] = c;
			buf[++cur] = '\0';
		}

		printf("\r\033[K%s%s", prefix, buf);
	}
}

void print_message(char *sender, char *body) {
	printf("\r\033[K<%s>: %s\n", sender, body);
}

void print_system_message(char *sender, char *body) {
	printf("\r\033[K=== %s: %s ===\n", sender, body);
}

void print_error_message(char *sender, char *body, int code) {
	printf("\r\033[K=== %s: %s === %d ===\n", sender, body, code);
}

void print_client_connected_message(char *client_name) {
	printf("\r\033[K=== %s connected ===\n", client_name);
}

void print_client_disconnected_message(char *client_name) {
	printf("\r\033[K=== %s disconnected ===\n", client_name);
}

void print_client_exists_message(char *client_name) {
	printf("\r\033[K=== %s already connected ===\n", client_name);
}
