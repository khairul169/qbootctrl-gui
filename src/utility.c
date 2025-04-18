#include <string.h>
#include <stdio.h>
#include <sys/utsname.h>
#include "utility.h"

char *get_slot_name(int slot)
{
    switch (slot)
    {
    case 0:
        return "A";
    case 1:
        return "B";
    default:
        return "N/A";
    }
}

void trim_quotes(char *str)
{
    size_t len = strlen(str);
    if (len >= 2 && str[0] == '"' && str[len - 1] == '"')
    {
        memmove(str, str + 1, len - 2);
        str[len - 2] = '\0';
    }
}

void get_sys_details(struct sys_details_t *out)
{
    const int BUF_SIZE = 256;
    char pretty_name[BUF_SIZE];
    strcpy(pretty_name, "Unknown");

    FILE *fp = fopen("/etc/os-release", "r");
    if (fp != NULL)
    {
        char line[BUF_SIZE];
        while (fgets(line, sizeof(line), fp))
        {
            if (strncmp(line, "PRETTY_NAME=", 12) == 0)
            {
                strncpy(pretty_name, line + 12, BUF_SIZE - 1);
                pretty_name[strcspn(pretty_name, "\n")] = '\0';
                trim_quotes(pretty_name);
                break;
            }
        }
        fclose(fp);
    }

    struct utsname uname_data;
    char kernel_name[BUF_SIZE];
    if (uname(&uname_data) == 0)
    {
        snprintf(kernel_name, BUF_SIZE, "%s %s", uname_data.sysname, uname_data.release);
    }

    // copy to struct
    strncpy(out->distro, pretty_name, MAX_STR_LEN - 1);
    out->distro[MAX_STR_LEN - 1] = '\0';

    strncpy(out->kernel, kernel_name, MAX_STR_LEN - 1);
    out->kernel[MAX_STR_LEN - 1] = '\0';
}
