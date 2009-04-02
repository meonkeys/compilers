#include <symtab.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lexer3.h>
#include <y.tab.h>

semrec_t *sym_table;

void
init_sym_table (void)
{
    /* these are library functions, not keywords... do we need another type? */
    semrec_t* tmp = new_semrec("read");
    tmp->type = TYPE_KEYWORD;
    putsym (tmp);
    tmp = new_semrec("write");
    tmp->type = TYPE_KEYWORD;
    putsym (tmp);
    tmp = new_semrec("fwrite");
    tmp->type = TYPE_KEYWORD;
    putsym (tmp);
    tmp = new_semrec("i");
    tmp->type = TYPE_INT;
    putsym (tmp);

    /*
     * FIXME: we need to add the real keywords
     * but I don't have that defined yet
     */
}

void
destroy_sym_table (void)
{
    semrec_t *cur = sym_table;
    semrec_t *next = NULL;
    while (cur != (semrec_t *) 0)
    {
        next = (semrec_t *) cur->next;
        free (cur->name);
        free (cur);
        cur = next;
    };
}

semrec_t *
new_semrec (char const *sym_name)
{
    semrec_t *ptr;
    ptr = (semrec_t *) malloc (sizeof (semrec_t));
    ptr->name = (char *) calloc (strlen (sym_name) + 1, 1);
    strcpy (ptr->name, sym_name);
    ptr->value.fval = 0;         /* Set value to 0 even if fctn.  */
    ptr->is_declared = FALSE;
    ptr->is_declared = FALSE;
    return ptr;
}

semrec_t *
putsymlist (semrec_t *item, type_t type)
{
    semrec_t* head = item;
    /* TODO: add check for existing symrec_ts with getsym */
    for (; item != (semrec_t *) 0; item = (semrec_t *) item->next)
    {
        /* TODO: later */
    }
    return head;
}

semrec_t *
putsym (semrec_t * ptr)
{
    ptr->next = (semrec_t *) sym_table;
    sym_table = ptr;
    return ptr;
}

semrec_t *
getsym (char const *sym_name)
{
    semrec_t *ptr;
    for (ptr = sym_table; ptr != (semrec_t *) 0; ptr = (semrec_t *) ptr->next)
    {
        if (strcmp (ptr->name, sym_name) == 0)
        {
            return ptr;
        }
    }
    return 0;
}

void
dump_symtab (void)
{
    semrec_t *ptr;
    printf ("dumping symbol table\n");
    for (ptr = sym_table; ptr != (semrec_t *) 0; ptr = (semrec_t *) ptr->next)
    {
        printf ("\tname = %s\n", ptr->name);
        printf ("\t\ttype = %d\n", ptr->type);
        if (ID == ptr->type)
        {
            printf ("\t\tvalue = %g\n", ptr->value.fval);
        }
        printf ("\t\tnext = %p\n", (void *) ptr->next);
    }
}
