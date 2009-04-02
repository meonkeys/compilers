#include <ourtypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <symtab.h>

/*
 * We're only doing type checking.  We could do constant folding
 * somewhere around here if we wanted.
 * TODO: write an int2float() and figure out where to call it
 */
semrec_t *
arith_op_type_reduce (semrec_t * t1, semrec_t * t2)
{
    semrec_t *ct = new_semrec("");
	ct->is_temp = TRUE;

	/*if((TRUE == t1->is_const) && (t2->is_const == TRUE)){ */
	printf("v1 = %d\tv2 = %d\n", t1->value.intval, t2->value.intval);
	printf("t1 = %d\tt2 = %d\n", t1->type, t2->type);
	/*} */

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
