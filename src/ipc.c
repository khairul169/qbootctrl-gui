#include "ipc.h"

int g_daemon_fd = -1;

// Start the IPC server
int ipc_start()
{
    int server_fd;
    struct sockaddr_un addr;
    char buf[256];

    unlink(SOCKET_PATH); // remove if exists

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0)
        return -1;

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) < 0)
        return -1;

    listen(server_fd, 1);

    return server_fd;
}

// Connect to the IPC server
int ipc_connect()
{
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1)
    {
        return -1;
    }

    return sock;
}

// Read a string
char *ipc_read(int sock_fd)
{
    char buffer[256];
    int len = read(sock_fd, buffer, sizeof(buffer) - 1);
    if (len <= 0)
        return NULL;

    buffer[len] = '\0';
    return strdup(buffer);
}

// Write a string
int ipc_write(int sock_fd, const char *str)
{
    return write(sock_fd, str, strlen(str));
}

// Send an int
int ipc_send_int(int sock, int value)
{
    return send(sock, &value, sizeof(int), 0);
}

// Receive an int
int ipc_recv_int(int sock, int *value)
{
    return recv(sock, value, sizeof(int), 0);
}

// Send a fixed-size struct
int ipc_send_struct(int sock, const void *data, size_t size)
{
    return send(sock, data, size, 0);
}

// Receive a fixed-size struct
int ipc_recv_struct(int sock, void *out, size_t size)
{
    return recv(sock, out, size, 0);
}
