/* Infix notation calculator with variables.  */

%{
/* System headers */
#include <math.h>
#include <stdio.h>
#include <y.tab.h>

/* Custom headers */
#include <scanner.h>

void yyerror (char const *);
%}

%defines
%output="y.tab.c"

/* Bison declarations.  */
%token NUM
%token ID
%right '='
%left '-' '+'
%left '*' '/'
%left NEG     /* negation--unary minus */
%right '^'    /* exponentiation */

%% /* The grammar follows.  */

input:    /* empty */
        | input line
;

line:     '\n'
        | exp '\n'  { printf ("\t%.10g\n", $1); }
;

exp:      NUM                { $$ = $1;         }
        | ID '=' exp
        | exp '+' exp        { $$ = $1 + $3;    }
        | exp '-' exp        { $$ = $1 - $3;    }
        | exp '*' exp        { $$ = $1 * $3;    }
        | exp '/' exp        { $$ = $1 / $3;    }
        | '-' exp  %prec NEG { $$ = -$2;        }
        | exp '^' exp        { $$ = pow ($1, $3); }
        | '(' exp ')'        { $$ = $2;         }
;

%%

