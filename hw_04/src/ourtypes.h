#ifndef OURTYPES_H
#define OURTYPES_H

#include <stdlib.h>

typedef enum TRUTH
{
    FALSE = 0,
    TRUE = 1
} truth_t;

typedef enum TYPE_T
{
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_VOID,
    TYPE_ARRAY,
    TYPE_FUNCTION,
    TYPE_STRUCT,
    TYPE_UNION,
    TYPE_ID
} type_t;

typedef struct array_s
{
    struct semrec_s *dimension;
} array_t;

typedef struct func_s
{
    type_t return_type;
    int num_params;
    struct semrec_s *param_list;
} func_t;

typedef struct struct_s
{
    char *tag;                  /* string before the structure/union */
    /* char *alias; *//* string after the structure/union */
    /* type_t type; *//* use the type_t type in parent semrec */
    struct semrec_s *member_list;
} struct_t;


/* Data type for links in the chain of symbols.  */
typedef struct semrec_s
{
    char *name;                 /* name of symbol */
    type_t type;
    int scope;                  /* what scope are we in */
    truth_t is_const;           /* is this a constant? */
    truth_t is_temp;            /* is this record restricted to the stack? */
    truth_t is_declared;        /* has this ID been declared? */
    union
    {
        int intval;
        double fval;
        char *stringval;
        array_t *arrayval;
        func_t *funcval;
        struct_t *structval;
    } value;
    struct semrec_s *next;      /* link field */
} semrec_t;

semrec_t *arith_op_type_reduce (semrec_t * t1, semrec_t * t2);

struct_t *newstructval (const char *tag, semrec_t * members);

semrec_t *newstruct (const char *tag, semrec_t * members, const char *name,
                     type_t type);

/* FIXME: This puts all the structs in the symbol table as well.  Probably too much going on here. */
void newstructlist (const char *tag, semrec_t * members,
                    semrec_t * names_list, type_t type);

void break_from_symtab(int scope);

#endif

/*
vim: expandtab shiftwidth=4 tabstop=4 smarttab
*/
