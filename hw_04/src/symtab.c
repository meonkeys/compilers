#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Custom Headers */
#include <lexer3.h>
#include <symtab.h>
#include <util.h>
#include <y.tab.h>

semrec_t *sym_table;

void
init_sym_table (void)
{
    /* these are library functions, not keywords... do we need another type? */
    semrec_t *tmp = new_semrec ("read");
    tmp->type = TYPE_KEYWORD;
    putsym (tmp);
    tmp = new_semrec ("write");
    tmp->type = TYPE_KEYWORD;
    putsym (tmp);
    tmp = new_semrec ("fwrite");
    tmp->type = TYPE_KEYWORD;
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
    while (cur != NULL)
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
    ptr->value.fval = 0;        /* Set value to 0 even if fctn.  */
    ptr->is_declared = FALSE;
    ptr->is_const = FALSE;
    ptr->is_temp = FALSE;
    ptr->next = NULL;
    /* printf("made record %s\n", ptr->name); */
    return ptr;
}

void
putsymlist (semrec_t * item, type_t type)
{
    semrec_t *head = item;
    /* TODO: add check for existing symrec_ts with getsym */
    while (head != NULL)
    {
        /* TODO: needs a better check for scoping */
        item->type = type;
        head = (semrec_t *) item->next;
        putsym (item);
        item = head;
    }
}

semrec_t *
putsym (semrec_t * ptr)
{
    semrec_t *sym = NULL;

    /* printf("trying to add %s\n", ptr->name); */
    sym = getsym (ptr->name);
    if (NULL == sym)
    {
        ptr->next = (semrec_t *) sym_table;
        sym_table = ptr;
    }
    else
    {
        /* FIXME: reporting and recovering from redeclared variables must
         * happen within the parser using yyerror, YYERROR, yyerrok, etc. */
        printf ("[FIXME] ID (%s) redeclared.\n", sym->name);
        ptr->is_temp = TRUE;
        our_free (ptr);
    }

    if(TYPE_STRUCT == ptr->type){
        printf("added struct w/ name: %s\ttag: %s\n", ptr->name, ptr->value.structval->tag);
    }

    return ptr;
}

semrec_t *
getsym (char const *sym_name)
{
    semrec_t *ptr;
    for (ptr = sym_table; ptr != NULL; ptr = ptr->next)
    {
        if (strcmp (ptr->name, sym_name) == 0)
        {
            return ptr;
        }
    }
    return NULL;
}

int
list_length (semrec_t * list)
{
    int num_items = 0;
    for (; list != NULL; list = list->next)
        num_items++;
    return num_items;
}

void
dump_symtab (void)
{
    semrec_t *ptr;
    printf ("dumping symbol table\n");
    for (ptr = sym_table; ptr != NULL; ptr = ptr->next)
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

/*
vim: expandtab shiftwidth=4 tabstop=4 smarttab
*/
