/* Infix notation calculator with variables.  */

%{
/* System headers */
#include <math.h>
#include <stdio.h>

/* Custom headers */
#include <ourtypes.h>
#include <y.tab.h>

/* Prototypes */
int yylex (void);
void yyerror (char const *);
%}

%union {
    double val;       /* For returning numbers.  */
    struct symrec_s *tptr;   /* For returning symbol-table pointers.  */
}

%defines
%output="y.tab.c"

/* Bison declarations.  */
%token END_OF_LINE
%token <val> NUM
%token <tptr> ID KEYWORD
%type <val> exp
%right '='
%left '-' '+'
%left '*' '/'
%left NEG     /* negation--unary minus */
%right '^'    /* exponentiation */

%%

input:  /* empty */
        | input line
;

line:   exp END_OF_LINE      { printf ("\t%.10g\n", $1) }
        | error END_OF_LINE  { yyerrok }
;

exp:    NUM                  { $$ = $1; }
        | ID                 { $$ = $1->value.var }
        | ID  '=' exp        {
                                if (KEYWORD == $1->type) {
                                    yyerror("keyword in lvalue"); YYERROR;
                                }
                                $$ = $3; $1->value.var = $3
                             }
        | exp '+' exp        { $$ = $1 + $3 }
        | exp '-' exp        { $$ = $1 - $3 }
        | exp '*' exp        { $$ = $1 * $3 }
        | exp '/' exp        { $$ = $1 / $3 }
        | '-' exp  %prec NEG { $$ = -$2 }
        | exp '^' exp        { $$ = pow ($1, $3) }
        | '(' exp ')'        { $$ = $2 }
;

%%
