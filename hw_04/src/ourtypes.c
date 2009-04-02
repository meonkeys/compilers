#include <stdlib.h>
#include <stdio.h>

/* Custom Headers */
#include <ourtypes.h>
#include <symtab.h>
#include <util.h>

/*
 * We're only doing type checking.  We could do constant folding
 * somewhere around here if we wanted.
 * TODO: write an int2float() and figure out where to call it
 */
semrec_t *
arith_op_type_reduce (semrec_t * t1, semrec_t * t2)
{
    semrec_t *ct = new_semrec ("");
    ct->is_temp = TRUE;

    /* set it to the "shared" type */
    ct->type = t1->type;

    if (FALSE == typecmp (t1->type, t2->type))
    {
        /*
         * One float - one int
         * Need to upconvert to float
         */
        ct->type = TYPE_FLOAT;
    }

    return ct;
}

/*
vim: expandtab shiftwidth=4 tabstop=4 smarttab
*/
