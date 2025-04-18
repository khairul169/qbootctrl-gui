#include <sys/wait.h>
#include "ipc.h"
#include "ui.h"
#include "daemon.h"
#include "bootctrl.h"

int main(int argc, char *argv[])
{
    // Executable started as a daemon
    if (argc >= 2 && strcmp(argv[1], "start-daemon") == 0)
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork failed");
            exit(1);
        }
        if (pid == 0)
        {
            return init_daemon(argc, argv);
        }

        return 0;
    }

    // Initialize IPC
    int sock = ipc_start();
    if (sock < 0)
    {
        perror("ipc_start");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    // Spawn daemon as root
    if (pid == 0)
    {
        printf("Running as root\n");
        char *args[] = {"pkexec", argv[0], "start-daemon", NULL};
        execvp(args[0], args);
        perror("execlp");
        exit(EXIT_FAILURE);
    }
    else
    {
        // wait for pkexec to finish
        int status;
        waitpid(pid, &status, 0);

        if (status != 0)
        {
            close(sock);
            perror("pkexec failed");
            exit(EXIT_FAILURE);
        }
    }

    // Wait for daemon to connect to ipc
    g_daemon_fd = accept(sock, NULL, NULL);
    if (g_daemon_fd < 0)
    {
        close(sock);
        perror("wait for daemon");
        exit(EXIT_FAILURE);
    }

    // Initialize UI
    init_ui(argc, argv);

    // Cleanup
    close(sock);

    return 0;
}
