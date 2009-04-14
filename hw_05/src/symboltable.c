#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "header.h"

#define TABLE_SIZE	1024


symtab *hash_table[TABLE_SIZE];



extern int scope;
extern int STRUCT_DEC;
extern int linenumber;
extern int offset;

int
HASH (char *str)
{
    int idx = 0;
    while (*str)
    {
        idx = idx << 1;
        idx += *str;
        str++;
    }
    return (idx & (TABLE_SIZE - 1));
}

symtab *
lookup (char *name)
{
    int hash_key;
    symtab *symptr;
    if (!name)
        return NULL;
    hash_key = HASH (name);
    symptr = hash_table[hash_key];

    while (symptr)
    {
        if (!(strcmp (name, symptr->lexeme)))
            return symptr;
        symptr = symptr->front;
    }
    return NULL;
}


symtab*
insert (char *name, TYPE type, void *P, IS_TYPE_DEF TypeDef)
{
    int hash_key;
    symtab *ptr;
    symtab *symptr = (symtab *) malloc (sizeof (symtab));

    hash_key = HASH (name);
    ptr = hash_table[hash_key];

    if (ptr == NULL)
    {
        /*first entry for this hash_key */
        hash_table[hash_key] = symptr;
        symptr->front = NULL;
        symptr->back = symptr;
    }
    else
    {
        symptr->front = ptr;
        ptr->back = symptr;
        symptr->back = symptr;
        hash_table[hash_key] = symptr;
    }

    symptr->scope = scope;
    symptr->type = type;
    symptr->type_when_def = TypeDef;
    symptr->lexeme = name;
    symptr->line = linenumber;
    switch (type)
    {
    case INT_:
        if (NULL != P)
            symptr->val_u.intval = ((init_id *)P)->val_u.intval;
        break;
    case FLOAT_:
        if (NULL != P)
            symptr->val_u.fval = ((init_id *)P)->val_u.fval;
        break;
    case ARR_:
        symptr->symtab_u.st_arr = P;
        break;
    case STR_:
        if (STRUCT_DEC)
            symptr->scope = -scope;
        symptr->symtab_u.st_struct = P;
        break;
    case FUNC_:
        symptr->symtab_u.st_func = P;
        symptr->scope = 0;
        break;
    case STR_VAR_:
        symptr->symtab_u.type_name = P;
        break;
    case TYPEDEF_:
        switch (TypeDef)
        {
        case TYPEDEF_ARR:
            symptr->symtab_u.st_arr = P;
            break;
        case TYPEDEF_STR:
            symptr->symtab_u.type_name = P;
            break;
        default:
            break;
        }

    default:
        break;
    }

    return symptr;
}


int
delete_scope (int scp)
{
    int i;
    symtab *ptr, *ptr1;

    for (i = 0; i < TABLE_SIZE; i++)
    {
        ptr = hash_table[i];
        if (ptr == NULL)
            continue;
        ptr1 = ptr;

        while ((ptr) && (abs (ptr->scope) == (scp)))
            ptr = ptr->front;

        if (ptr1 == ptr)
            continue;

        hash_table[i] = ptr;
        if (ptr)
            ptr->back = ptr;

    }

    return 1;
}

/*
vim: expandtab shiftwidth=4 tabstop=4 smarttab
*/
