/* Infix notation calculator with variables.  */

%{
/* System headers */
#include <math.h>
#include <stdio.h>
#include <y.tab.h>

/* Custom headers */

/* Prototypes */
int yylex(void);
void yyerror (char const *);
%}

%union {
    int num;
    float fnum;
}

%defines
%output="y.tab.c"

/* Bison declarations.  */
%token END_OF_LINE
%token <num> INT
%token <fnum> FLOAT
%token ID
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

exp:      INT                { $<num>$ = $<num>1; }
        | exp '+' exp        { $<num>$ = $<num>1 + $<num>3;    }
        | exp '-' exp        { $<num>$ = $<num>1 - $<num>3;    }
        | exp '*' exp        { $<num>$ = $<num>1 * $<num>3;    }
        | exp '/' exp        { $<num>$ = $<num>1 / $<num>3;    }
        | '-' exp  %prec NEG { $<num>$ = -$<num>2; }
        | exp '^' exp        { $<num>$ = pow ($<num>1, $<num>3); }
        | '(' exp ')'        { $<num>$ = $<num>2; }
;

%%

