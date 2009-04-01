#include <ourtypes.h>
#include <stdlib.h>

/*
 * We're only doing type checking.  We could do constant folding
 * somewhere around here if we wanted.
 * TODO: write an int2float() and figure out where to call it
 */
semrec_t *
arith_op_type_reduce (semrec_t * t1, semrec_t * t2)
{
    semrec_t *ct = (semrec_t *) malloc (sizeof (semrec_t));

    /* set it to the "shared" type */
    ct->type = t1->type;

    if ((t1->type != t2->type))
    {
        /*
         * One float - one int
         * Need to upconvert to float
         */
        ct->type = TYPE_FLOAT;
    }

    return ct;
}
