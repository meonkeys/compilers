#include <symtab.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lexer3.h>
#include <y.tab.h>

symrec_t *sym_table;

void
init_sym_table (void)
{
    /* these are library functions, not keywords... do we need another type? */
    putsym ("read", STYPE_KEYWORD);
    putsym ("write", STYPE_KEYWORD);
    putsym ("fwrite", STYPE_KEYWORD);

    /*
     * FIXME: we need to add the real keywords
     * but I don't have that defined yet
     */
}

void
destroy_sym_table (void)
{
    symrec_t *cur = sym_table;
    symrec_t *next = NULL;
    while (cur != (symrec_t *) 0)
    {
        next = (symrec_t *) cur->next;
        free (cur->name);
        free (cur);
        cur = next;
    };
}

symrec_t *
putsym (char const *sym_name, int sym_type)
{
    symrec_t *ptr;
    ptr = (symrec_t *) malloc (sizeof (symrec_t));
    ptr->name = (char *) calloc (strlen (sym_name) + 1, 1);
    strcpy (ptr->name, sym_name);
    ptr->type = sym_type;
    ptr->value.var = 0;         /* Set value to 0 even if fctn.  */
    ptr->is_declared = FALSE;
    ptr->next = (symrec_t *) sym_table;
    sym_table = ptr;
    return ptr;
}

symrec_t *
getsym (char const *sym_name)
{
    symrec_t *ptr;
    for (ptr = sym_table; ptr != (symrec_t *) 0; ptr = (symrec_t *) ptr->next)
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
    symrec_t *ptr;
    printf ("dumping symbol table\n");
    for (ptr = sym_table; ptr != (symrec_t *) 0; ptr = (symrec_t *) ptr->next)
    {
        printf ("\tname = %s\n", ptr->name);
        printf ("\t\ttype = %d\n", ptr->type);
        if (ID == ptr->type)
        {
            printf ("\t\tvalue = %g\n", ptr->value.var);
        }
        printf ("\t\tnext = %p\n", (void *) ptr->next);
    }
}
