#ifndef OURTYPES_H
#define OURTYPES_H

#include <stdlib.h>

typedef enum TRUTH
{
    FALSE = 0,
    TRUE = 1
} truth;

typedef enum TYPE_T
{
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_VOID,
    TYPE_ID,
    TYPE_KEYWORD
} type_t;

/* Function type.  */
typedef double (*func_t) (double);

/* Data type for links in the chain of symbols.  */
typedef struct semrec_s
{
    char *name;                 /* name of symbol */
    enum TYPE_T type;        /* type of symbol: SYMTYPE_KEYWORD, SYMTYPE_ID, etc. */
    truth is_declared;          /* has this ID been declared? */
    union
    {
        func_t fnctptr;         /* value of a FNCT */
        int intval;
        double fval;
        char *stringval;
    } value;
    struct semrec_s *next;      /* link field */
} semrec_t;

semrec_t *arith_op_type_reduce (semrec_t * t1, semrec_t * t2);

#endif
