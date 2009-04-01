#include <util.h>
#include <stdlib.h>

void
free_const (semrec_t * bt)
{
    if (bt->type == TYPE_STRING)
    {
        free (bt->value.stringval);
    }
    free (bt);
}
