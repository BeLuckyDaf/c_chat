#ifndef C_CHAT_TERMIN_H
#define C_CHAT_TERMIN_H

#include "common.h"

void enable_chat_mode();
void reset_chat_mode();

int read_line(char *buf, size_t bufsize, char *prefix);

void print_message(char *sender, char *body);
void print_system_message(char *sender, char *body);
void print_error_message(char *sender, char *body, int code);
void print_client_connected_message(char *client_name);
void print_client_disconnected_message(char *client_name);
void print_client_exists_message(char *client_name);

#endif //C_CHAT_TERMIN_H
