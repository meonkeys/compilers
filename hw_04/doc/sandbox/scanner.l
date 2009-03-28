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
%}

ID              [a-z][a-z0-9_]*
DIGIT           [0-9]
INT             {DIGIT}+
FLOAT           \.{DIGIT}+|{DIGIT}+\.|{DIGIT}+\.{DIGIT}+
ARITHOP         "+"|"-"|"*"|"/"|"^"
ASSIGNOP        "="
WHITESPACE      [ \t]+

%%

{ID}            { return ID; }
{INT}           { return NUM; }
{FLOAT}         { return NUM; }
{ASSIGNOP}      { return yytext[0]; }
{ARITHOP}       { return yytext[0]; }
{WHITESPACE}    {}
\n              {}
 .              { printf("Unrecognized character: %s on line: %d\n",
                         yytext, yylineno); }

%%

