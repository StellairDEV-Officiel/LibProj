#include "./include/port_knocking.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT_SEQUENCE {7777, 5921, 8675}
#define TIMEOUT 10  // Délai maximal entre les frappes (en secondes)

static int expected_sequence[SEQUENCE_LENGTH] = PORT_SEQUENCE;
static int client_knock_state[256] = {0};  // Suivi de l’état de chaque client (simple)

int is_knock_sequence_valid(int client_ip) {
    return client_knock_state[client_ip] == SEQUENCE_LENGTH;
}

/**
 * Écoute des ports pour la séquence de port knocking.
 */
void start_port_knocking_listener() {
    int sock[SEQUENCE_LENGTH];
    struct sockaddr_in addr[SEQUENCE_LENGTH];
    fd_set readfds;
    int max_fd = 0;

    // Création des sockets d'écoute pour chaque port de la séquence
    for (int i = 0; i < SEQUENCE_LENGTH; i++) {
        sock[i] = socket(AF_INET, SOCK_DGRAM, 0);  // Utilisation de UDP pour les knocks
        addr[i].sin_family = AF_INET;
        addr[i].sin_port = htons(expected_sequence[i]);
        addr[i].sin_addr.s_addr = INADDR_ANY;

        bind(sock[i], (struct sockaddr *)&addr[i], sizeof(addr[i]));
        if (sock[i] > max_fd) max_fd = sock[i];
    }

    printf("[+] Port knocking en attente sur les ports : 7777, 5921, 8675\n");

    while (1) {
        FD_ZERO(&readfds);
        for (int i = 0; i < SEQUENCE_LENGTH; i++) {
            FD_SET(sock[i], &readfds);
        }

        // Surveillance des frappes sur les ports
        if (select(max_fd + 1, &readfds, NULL, NULL, NULL) > 0) {
            for (int i = 0; i < SEQUENCE_LENGTH; i++) {
                if (FD_ISSET(sock[i], &readfds)) {
                    struct sockaddr_in client_addr;
                    socklen_t addr_len = sizeof(client_addr);
                    char buffer[1];  // Paquet vide

                    recvfrom(sock[i], buffer, sizeof(buffer), 0, 
                             (struct sockaddr *)&client_addr, &addr_len);

                    int client_ip = client_addr.sin_addr.s_addr;

                    if (client_knock_state[client_ip] == i) {
                        client_knock_state[client_ip]++;
                        printf("Knock %d reçu de %s\n", i + 1, inet_ntoa(client_addr.sin_addr));

                        if (client_knock_state[client_ip] == SEQUENCE_LENGTH) {
                            printf("[+] Séquence correcte pour %s ! Connexion autorisée.\n", inet_ntoa(client_addr.sin_addr));
                        }
                    } else {
                        client_knock_state[client_ip] = 0;  // Réinitialiser si la séquence est incorrecte
                    }
                }
            }
        }
    }
}
