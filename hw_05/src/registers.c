#include <stdlib.h>
#include <stdio.h>
#include <header.h>

#define REG_COUNT 32

int reg = 8;

typedef enum
{ FREE = 0, NOTSAVED = 1, SAVED = 2 } reg_status;


/* FIXME: What type to put here? */
/* TODO: use whatever the container is to hold some sort of 
 * association between records and registers */
/* regtable[REG_COUNT];*/

int reg_costs[REG_COUNT];

int
get_reg (var_ref * vr)
{
    if (NULL == vr || 0 == vr->place)
    {
        int i;
        /* grab the first free one */

        for (i = 8; i < 16; i++)
        {
            if (reg_costs[i] == 0)
            {
                reg_costs[i] = 1;
                return i;
            }
        }

        /* no free ones, grab the first saved one */
        for (i = 8; i < 16; i++)
        {
            if (reg_costs[i] == 1)
            {
                return i;
            }
        }

        /* TODO: handle the spill case! */
        return 8;
    }
    else
    {
        return vr->place;
    }
}

int
get_result_reg ()
{
    return reg++;
}

void
free_reg (int r)
{
    reg_costs[r] = 0;
}

void
ns_reg (int r)
{
    reg_costs[r] = 1;
}

void
save_reg (int r)
{
    reg_costs[r] = 2;
}

/*
vim: expandtab shiftwidth=4 tabstop=4 smarttab
*/
