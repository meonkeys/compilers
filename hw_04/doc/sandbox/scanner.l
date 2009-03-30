%option warn nodefault
%option noinput nounput noyywrap yylineno
/* %option debug */
%option header-file="scanner.h"

%top{
/* For fileno(3). See feature_test_macros(7) manpage. */
#define _GNU_SOURCE
}

%{
/* System headers */
#include <assert.h>
#include <string.h> /* for memcmp() */
/* Custom headers */
#include <y.tab.h>
#include <ourtypes.h>
%}

ID              [a-z][a-z0-9_]*
DIGIT           [0-9]
INT             {DIGIT}+
FLOAT           \.{DIGIT}+|{DIGIT}+\.|{DIGIT}+\.{DIGIT}+
ARITHOP         "+"|"-"|"*"|"/"|"^"
ASSIGNOP        "="
PAREN           "("|")"
WHITESPACE      [ \t]+

%%

{ID}            { return ID; }
{INT}           {
                    yylval.const_val.type = CONST_INT;
                    yylval.const_val.int_val = atoi(yytext);
                    return CONST;
                }
{FLOAT}         {
                    yylval.const_val.type = CONST_FLOAT;
                    yylval.const_val.float_val = atof(yytext);
                    return CONST;
                }
{ASSIGNOP}      { return '='; }
{ARITHOP}       { return yytext[0]; }
{PAREN}         { return yytext[0]; }
{WHITESPACE}    {}
\n              { return END_OF_LINE; }
 .              { printf("Unrecognized character: %s on line: %d\n",
                         yytext, yylineno); }

%%

