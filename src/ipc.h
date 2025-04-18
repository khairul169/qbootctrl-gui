#ifndef IPC_H
#define IPC_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/qbootctl-gtk.sock"

extern int g_daemon_fd;

int ipc_start();
int ipc_connect();
char *ipc_read(int sock_fd);
int ipc_write(int sock_fd, const char *str);
int ipc_send_int(int sock, int i);
int ipc_recv_int(int sock, int *value);
int ipc_recv_struct(int sock, void *out, size_t size);
int ipc_send_struct(int sock, const void *data, size_t size);

#endif
