#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "daemon.h"
#include "ipc.h"
#include "bootctrl.h"

const struct boot_control_module *impl = &bootctl;

int get_slot_info(struct slot_info *slots)
{
    int rc;
    int active_slot = impl->getActiveBootSlot();

    for (int i = 0; i < 2; i++)
    {
        slots[i].active = i == active_slot;

        rc = impl->isSlotMarkedSuccessful(i);
        if (rc < 0)
            return rc;
        slots[i].successful = rc;

        rc = impl->isSlotBootable(i);
        if (rc < 0)
            return rc;
        slots[i].bootable = rc;
    }

    return 0;
}

int init_daemon(int argc, char *argv[])
{
    if (geteuid() != 0)
    {
        fprintf(stderr, "This program must be run as root!\n");
        return 1;
    }

    int sock = ipc_connect();
    if (sock < 0)
    {
        perror("daemon not running");
        return 1;
    }

    while (1)
    {
        char *msg = ipc_read(sock);
        if (msg == NULL)
            break;

        if (strcmp(msg, "get_slots") == 0)
        {
            struct slot_info slots[2] = {{0}};
            get_slot_info(slots);
            ipc_send_struct(sock, &slots, sizeof(slots));
        }

        // activate %d
        if (strncmp(msg, "activate", 8) == 0)
        {
            int id = atoi(msg + 9), res = -1;
            printf("Activating slot %d\n", id);

            if (id >= 0 && id < 2)
            {
                res = impl->setActiveBootSlot(id, false);
            }

            ipc_send_int(sock, res);
        }

        if (strcmp(msg, "reboot") == 0)
        {
            printf("Rebooting...\n");
            system("reboot");
            break;
        }

        if (strcmp(msg, "kill") == 0)
            break;

        free(msg);
    }

    close(sock);

    printf("Daemon stopped.\n");
    return 0;
}
