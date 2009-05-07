#include <stdlib.h>
#include <stdio.h>
#include <header.h>
#include <assert.h>

int reg = 8;

typedef enum
{ FREE = 0, NOTSAVED = 1, SAVED = 2 } reg_status;


/* FIXME: What type to put here? */
/* TODO: use whatever the container is to hold some sort of 
 * association between records and registers */
/* regtable[REG_COUNT];*/

reg_status reg_costs[REG_COUNT];
reg_status return_reg_costs[4];

int
get_reg (var_ref * vr)
{
    if (NULL == vr || vr->place < 1 || vr->place > REG_COUNT)
    {
        return get_result_reg ();
    }
    else if (NULL != vr && vr->place < MAX_REG && vr->place > 1)
    {
        /*fprintf(stderr, "get_reg = %d\n", vr->place); */
        reg_costs[vr->place] = NOTSAVED;        /* I think this is the right cost to set */
        return vr->place;
    }
    else
    {                           /* this should never happen */
        assert (0);
        return 8;
    }
}

int
get_result_reg (void)
{
    int i;
    /* grab the first free one */

    for (i = 8; i < MAX_REG; i++)
    {
        if (reg_costs[i] == FREE)
        {
            /*fprintf(stderr, "$%d is stealing regs\n", i); */
            reg_costs[i] = NOTSAVED;
            return i;
        }
    }

    /* no free ones, grab the first saved one */
    for (i = 8; i < MAX_REG; i++)
    {
        if (reg_costs[i] == NOTSAVED)
        {
            /* FIXME: don't set SAVED */
            reg_costs[i] = SAVED;
            return i;
        }
    }

    for (i = 8; i < MAX_REG; i++)
    {
        if (reg_costs[i] == SAVED)
        {
            return i;
        }
    }

    /* The spill case!  We return the stack pointer */
    return 29;
}

int
get_return_reg(var_ref* v){
    int i;

    if(FLOAT_ == v->type){
        /* grab the first free one */

        for (i = 0; i < 2; i++)
        {
            if (return_reg_costs[i] == FREE)
            {
                return_reg_costs[i] = NOTSAVED;
                return i;
            }
        }

        /* no free ones, grab the first saved one */
        for (i = 0; i < 2; i++)
        {
            if (return_reg_costs[i] == NOTSAVED)
            {
                /* FIXME: don't set SAVED */
                return_reg_costs[i] = SAVED;
                return i;
            }
        }

        for (i = 0; i < 2; i++)
        {
            if (return_reg_costs[i] == SAVED)
            {
                return i;
            }
        }
    }
    else if(INT_ == v->type){
        /* grab the first free one */

        for (i = 2; i < 4; i++)
        {
            if (return_reg_costs[i] == FREE)
            {
                return_reg_costs[i] = NOTSAVED;
                return i;
            }
        }

        /* no free ones, grab the first saved one */
        for (i = 2; i < 4; i++)
        {
            if (return_reg_costs[i] == NOTSAVED)
            {
                /* FIXME: don't set SAVED */
                return_reg_costs[i] = SAVED;
                return i;
            }
        }

        for (i = 2; i < 4; i++)
        {
            if (return_reg_costs[i] == SAVED)
            {
                return i;
            }
        }
    }

    /* bad things */
    return 0;
}

void
free_reg (int r)
{
    if (r >= 8 && r < MAX_REG)
    {
        /*fprintf(stderr, "freeing $%d\n", r); */
        reg_costs[r] = FREE;
    }
}

void
ns_reg (int r)
{
    /*fprintf(stderr, "setting as ns $%d\n", r); */
    reg_costs[r] = NOTSAVED;
}

void
save_reg (int r)
{
    reg_costs[r] = SAVED;
}

void
print_regs ()
{
    int i;

    for (i = 8; i < MAX_REG; i++)
    {
        fprintf (stderr, "%d:%d ", i, reg_costs[i]);
    }
    fprintf (stderr, "\n");
}

/*
vim: expandtab shiftwidth=4 tabstop=4 smarttab
*/
