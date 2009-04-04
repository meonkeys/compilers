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
        else if (TYPE_STRUCT == bt->type)
        {
            if (NULL != bt->value.structval)
            {
                if (NULL != bt->value.structval->tag)
                {
                    free (bt->value.structval->tag);
                }
                if (NULL != bt->value.structval->member_list)
                {
                    our_free_list (bt->value.structval->member_list);
                }
                free (bt->value.structval);
            }
        }
        free (bt);
    }
}

void
our_free_list (semrec_t * rec)
{
    semrec_t *cur = rec;
    semrec_t *next = NULL;
    while (cur != NULL)
    {
        cur->is_temp = TRUE;
        next = (semrec_t *) cur->next;
        our_free (cur);
        cur = next;
    };

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

/*
vim: expandtab shiftwidth=4 tabstop=4 smarttab
*/
