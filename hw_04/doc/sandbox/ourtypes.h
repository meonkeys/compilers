#ifndef OURTYPES_H
#define OURTYPES_H

typedef enum TRUTH {
    FALSE = 0,
    TRUE = 1
} truth;

/* Function type.  */
typedef double (*func_t) (double);

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

symrec_t *putsym (char const *, int);
symrec_t *getsym (char const *);
void dump_symtab (void);

#endif
