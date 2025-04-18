#ifndef UTILITY_H
#define UTILITY_H

#define MAX_STR_LEN 64

struct sys_details_t
{
    char distro[MAX_STR_LEN];
    char kernel[MAX_STR_LEN];
};

char *get_slot_name(int slot);
void get_sys_details(struct sys_details_t *details);

#endif