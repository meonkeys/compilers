#include <util.h>
#include <stdlib.h>
#include <stdio.h>

void
our_free (semrec_t * bt)
{
    if (TRUE == bt->is_const || TRUE == bt->is_temp)
    {
        free (bt->name);        /* don't know how well this holds */
        if (TYPE_STRING == bt->type)
        {
            free (bt->value.stringval);
        }
        free (bt);
    }
}

truth_t
typecmp (type_t t1, type_t t2)
{
    if (t1 == t2)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
