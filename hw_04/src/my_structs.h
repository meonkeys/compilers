#define INTC 0
#define FC 1
#define SC 2

typedef enum TRUTH {
    FALSE = 0,
    TRUE = 1
} truth;

/* Function type.  */
typedef double (*func_t) (double);

typedef struct Const_type{
	int const_type; /* 0: int, 1: float, 2: string */
	union{
		int intval;
		double fval;
		char* sc;
	} const_u;
} Const_type;

/* Data type for links in the chain of symbols.  */
typedef struct symrec_s
{
    char *name;                 /* name of symbol */
    int type;                   /* type of symbol: either ID, KEYWORD, or ? */
    truth is_declared;          /* has this ID been declared? */
    union
    {
        double var;             /* value of a VAR */
		int dimension;			/* value of a dimension */
        func_t fnctptr;         /* value of a FNCT */
    } value;
    struct symrec_s *next;      /* link field */
} symrec_t;

symrec_t *putsym (char const *, int);
symrec_t *getsym (char const *);
void dump_symtab (void);

