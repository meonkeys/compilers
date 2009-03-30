/* Infix notation calculator with variables.  */

%{
/* System headers */
#include <math.h>
#include <stdio.h>
#include <y.tab.h>

/* Custom headers */
#include <ourtypes.h>

/* Prototypes */
int yylex (void);
void yyerror (char const *);

const_val * arith_op(const_val *exp1, const_val *exp2, int op);
%}

%union {
    struct {
        int type;
        union {
            int int_val;
            double float_val;
            char *string_val;
        } const_u;
    } const_val;
}

%defines
%output="y.tab.c"

/* Bison declarations.  */
%token END_OF_LINE
%token <num> INT
%token <fnum> FLOAT
%token ID
%token <const_ptr> CONST
%right '='
%left '-' '+'
%left '*' '/'
%left NEG     /* negation--unary minus */
%right '^'    /* exponentiation */

%% /* The grammar follows.  */

input:  /* empty */
        | input line
;

line:   exp END_OF_LINE      { printf ("\t%d\n", $<num>1); }
;

exp:      CONST              { $<num>$ = $<num>1 }
        | ID                 { }
        | ID  '=' exp        { }
        | exp '+' exp        {
                               $<const_val>$ = arith_op(&$<const_val>1, &$<const_val>3, $2);
                             }
        | exp '-' exp        { $<num>$ = $<num>1 - $<num>3;    }
        | exp '*' exp        { $<num>$ = $<num>1 * $<num>3;    }
        | exp '/' exp        { $<num>$ = $<num>1 / $<num>3;    }
        | '-' exp  %prec NEG { $<num>$ = -$<num>2; }
        | exp '^' exp        { $<num>$ = pow ($<num>1, $<num>3); }
        | '(' exp ')'        { $<num>$ = $<num>2; }
;

%%

const_val *
arith_op(const_val *exp1, const_val *exp2, int op)
{
    return NULL;
}
