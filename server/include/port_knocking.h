#ifndef PORT_KNOCKING_H
#define PORT_KNOCKING_H

#define SEQUENCE_LENGTH 3

void start_port_knocking_listener();
int is_knock_sequence_valid(int client_ip);

#endif
