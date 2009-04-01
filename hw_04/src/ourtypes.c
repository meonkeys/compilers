#include <ourtypes.h>
#include <stdlib.h>

/*
 * We're only doing type checking.  We could do constant folding
 * somewhere around here if we wanted.
 * TODO: write an int2float() and figure out where to call it
 */
base_type *
arith_op_type_reduce (base_type * t1, base_type * t2)
{
    base_type *ct = (struct base_type *) malloc (sizeof (base_type));

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
