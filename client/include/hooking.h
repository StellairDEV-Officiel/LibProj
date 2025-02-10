#ifndef HOOKING_H
#define HOOKING_H

ssize_t intercepted_read(int fd, void *buf, size_t count);
ssize_t intercepted_write(int fd, const void *buf, size_t count);
int intercepted_open(const char *pathname, int flags, ...);

#endif
