#ifndef OURTYPES_H
#define OURTYPES_H

#include <stdlib.h>

typedef enum TRUTH {
    FALSE = 0,
    TRUE = 1
} truth;

typedef enum BASE_T{
	/* 
	 * I = int
	 * F = float
	 * S = string
	 * V = void
	 */
	I = 0,
	F = 1,
	S = 2,
	V = 3
} base_t;

/* Function type.  */
typedef double (*func_t) (double);

typedef struct base_type
{
	int type; /* 0: int, 1: float, 2: string, 3: void */
	union{
		int intval;
		double fval;
		char* sc;
	} const_u;
} base_type;

/* Data type for links in the chain of symbols.  */
typedef struct symrec_s
{
    char *name;                 /* name of symbol */
    int type;                   /* type of symbol: either ID, KEYWORD, or ? */
    truth is_declared;          /* has this ID been declared? */
    union
    {
        double var;             /* value of a VAR */
        func_t fnctptr;         /* value of a FNCT */
    } value;
    struct symrec_s *next;      /* link field */
} symrec_t;

base_type* arith_op_type_reduce(base_type* t1, base_type* t2);

#endif
