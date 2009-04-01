#ifndef OURTYPES_H
#define OURTYPES_H

#include <stdlib.h>

typedef enum TRUTH
{
    FALSE = 0,
    TRUE = 1
} truth;

typedef enum BASE_T
{
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_VOID
} base_t;

typedef enum SYMTYPE_T
{
    STYPE_ID,
    STYPE_KEYWORD
} sym_t;

/* Function type.  */
typedef double (*func_t) (double);

typedef struct base_type
{
    enum BASE_T type;
    union
    {
        int intval;
        double fval;
        char *sc;
    } const_u;
} base_type;

/* Data type for links in the chain of symbols.  */
typedef struct symrec_s
{
    char *name;                 /* name of symbol */
    enum SYMTYPE_T type;        /* type of symbol: SYMTYPE_KEYWORD, SYMTYPE_ID, etc. */
    truth is_declared;          /* has this ID been declared? */
    union
    {
        double var;             /* value of a VAR */
        func_t fnctptr;         /* value of a FNCT */
    } value;
    struct symrec_s *next;      /* link field */
} symrec_t;

base_type *arith_op_type_reduce (base_type * t1, base_type * t2);

#endif
