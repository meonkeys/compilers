#include <util.h>
#include <stdlib.h>

void
free_const (base_type * bt)
{
    if (bt->type == TYPE_STRING)
    {
        free (bt->const_u.sc);
    }
    free (bt);
}
