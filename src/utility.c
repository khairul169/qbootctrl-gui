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
