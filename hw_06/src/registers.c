#include <stdlib.h>
#include <stdio.h>
#include <header.h>

int reg = 8;

typedef enum
{ FREE = 0, NOTSAVED = 1, SAVED = 2 } reg_status;


/* FIXME: What type to put here? */
/* TODO: use whatever the container is to hold some sort of 
 * association between records and registers */
/* regtable[REG_COUNT];*/

reg_status reg_costs[REG_COUNT];

int
get_reg (var_ref * vr)
{
    if (NULL == vr || vr->place < 1 || vr->place > REG_COUNT)
    {
        return get_result_reg();
    }
    else if (NULL != vr && vr->place < REG_COUNT && vr->place > 0)
    {
        reg_costs[vr->place] = NOTSAVED;       /* I think this is the right cost to set */
        return vr->place;
    }
    else
    {                           /* this should never happen */
        return 8;
    }
}

int
get_result_reg (void)
{
    /*return reg++;*/
    int i;
    /* grab the first free one */

    for (i = 8; i < 25; i++)
    {
        if (reg_costs[i] == FREE)
        {
            reg_costs[i] = NOTSAVED;
            return i;
        }
    }

    /* no free ones, grab the first saved one */
    for (i = 8; i < 25; i++)
    {
        if (reg_costs[i] == NOTSAVED)
        {
            return i;
        }
    }

    for (i = 8; i < 25; i++)
    {
        if (reg_costs[i] == SAVED)
        {
            return i;
        }
    }

    /* The spill case!  We return the stack pointer */
    return 29;
}

void
free_reg (int r)
{
    reg_costs[r] = FREE;
}

void
ns_reg (int r)
{
    reg_costs[r] = NOTSAVED;
}

void
save_reg (int r)
{
    reg_costs[r] = SAVED;
}

/*
vim: expandtab shiftwidth=4 tabstop=4 smarttab
*/
