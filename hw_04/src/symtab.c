#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Custom Headers */
#include <lexer3.h>
#include <symtab.h>
#include <util.h>
#include <y.tab.h>

void
init_sym_table (void)
{
    semrec_t *tmp = new_semrec ("read");
    tmp->scope = 0;
    tmp->type = TYPE_FUNCTION;
    tmp->value.funcval = malloc (sizeof (func_t));
    putsym (tmp);
    tmp = new_semrec ("write");
    tmp->scope = 0;
    tmp->type = TYPE_FUNCTION;
    tmp->value.funcval = malloc (sizeof (func_t));
    putsym (tmp);
    tmp = new_semrec ("fwrite");
    tmp->scope = 0;
    tmp->type = TYPE_FUNCTION;
    tmp->value.funcval = malloc (sizeof (func_t));
    putsym (tmp);
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
    ptr->scope = 0;             /* FIXME: Review. Is this a sane initialization value? */
    ptr->value.fval = 0;        /* Set value to 0 even if fctn.  */
    ptr->is_declared = FALSE;
    ptr->is_const = FALSE;
    ptr->is_temp = FALSE;
    ptr->next = NULL;
    return ptr;
}

semrec_t *
putsymlist (semrec_t * list)
{
    semrec_t *return_value = list;
    semrec_t *head = list;

    while (head != NULL)
    {
        /* TODO: needs a better check for scoping */
        head = list->next;
        if (NULL == putsym (list)) {
            return_value = NULL;
        }
        list = head;
    }

    return return_value;
}

semrec_t *
putsym (semrec_t * ptr)
{
    semrec_t *sym = NULL;

    sym = getsym (ptr->name, ptr->scope);
    if (NULL == sym)
    {
        ptr->next = (semrec_t *) sym_table;
        sym_table = ptr;
    }
    else
    {
        strncpy(last_redeclared, ptr->name, MAX_ID_LENGTH - 1);
        ptr->is_temp = TRUE;
        our_free (ptr);
        ptr = NULL;
    }

    return ptr;
}

semrec_t *
getsym (char const *sym_name, int scope)
{
    semrec_t *ptr;
    for (ptr = sym_table; ptr != NULL; ptr = ptr->next)
    {
        if (strcmp (ptr->name, sym_name) == 0 && ptr->scope <= scope)
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
apply_type (semrec_t * list, type_t type)
{
    semrec_t *head = list;
    /* TODO: add check for existing symrec_ts with getsym */
    while (head != NULL)
    {
        /* TODO: needs a better check for scoping */
        head = list->next;
        list->type = type;
        list = head;
    }
}

void
apply_scope (semrec_t * list, int scope)
{
    semrec_t *head = list;
    /* TODO: add check for existing symrec_ts with getsym */
    while (head != NULL)
    {
        /* TODO: needs a better check for scoping */
        head = list->next;
        list->scope = scope;
        list = head;
    }
}

/*
 * The most recent scope is always at the head
 * of the list
 */
void
free_scope (int scope)
{
    semrec_t *head = sym_table;
    /* TODO: add check for existing symrec_ts with getsym */
    while (head->scope == scope)
    {
        /* TODO: needs a better check for scoping */
        head = sym_table->next;
        if (sym_table->scope == scope)
        {
            sym_table->is_temp = TRUE;
            our_free (sym_table);       /* only frees the head */
        }
        sym_table = head;
    }
}

/*
vim: expandtab shiftwidth=4 tabstop=4 smarttab
*/
