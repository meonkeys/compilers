#include <util.h>
#include <stdlib.h>

void
our_free (semrec_t * bt)
{
    if (TRUE == bt->is_const)
    {
        if (TYPE_STRING == bt->type)
        {
            free (bt->value.stringval);
        }
        free (bt);
    }
}
