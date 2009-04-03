#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

        /* Binary conversion. */
        if (TYPE_INT == t1->type)
        {
            t1->value.fval = t1->value.intval;
            t1->type = TYPE_FLOAT;
        }
        else
        {
            t2->value.fval = t2->value.intval;
            t2->type = TYPE_INT;
        }
    }

    return ct;
}

struct_t *
newstructval (const char *tag, semrec_t * members)
{
    struct_t *ptr = malloc (sizeof (struct_t));
    ptr->tag = malloc (sizeof (char) * strlen (tag) + 1);
    ptr->tag = strcpy (ptr->tag, tag);
    ptr->member_list = members;

    return ptr;
}

semrec_t *
newstruct (const char *tag, semrec_t * members, const char *name, type_t type)
{
    semrec_t *ptr = new_semrec (name);
    ptr->type = type;
    ptr->value.structval = newstructval (tag, members);
    return ptr;
}

void
newstructlist (const char *tag, semrec_t * members, semrec_t * names_list,
               type_t type)
{
    semrec_t *ptr;
    semrec_t *structptr;
    for (ptr = names_list; ptr != NULL; ptr = ptr->next)
    {
        structptr = newstruct (tag, members, ptr->name, type);
        putsym (structptr);
    }
}

/* This is weird, don't touch */
void
break_from_symtab(int scope){
    semrec_t *head = sym_table;
    /*fprintf(stderr, "freeing scope %d\n", scope);*/
    while (head->scope == scope)
    {
        head = sym_table->next;
        if(sym_table->scope != scope){
            /*fprintf(stderr, "freeing %s in %d\n", sym_table->name, scope);*/
            sym_table->next = NULL;
        }
        sym_table = head;
    }
}

/*
vim: expandtab shiftwidth=4 tabstop=4 smarttab
*/
