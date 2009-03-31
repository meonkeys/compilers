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
/* Prototypes */
#define MAX_ID_LENGTH 256
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

{ID}            {
                    int len = strlen (yytext);
                    symrec_t *tmp = NULL;
                    assert (len < MAX_ID_LENGTH);
                    tmp = getsym (yytext);
                    if (NULL == tmp) {
                        tmp = putsym (yytext, ID);
                        tmp->virgin = TRUE;
                    } else {
                        tmp->virgin = FALSE;
                    }
                    yylval.tptr = tmp;
                    return ID;
                }
{INT}           {
                    yylval.val = atof(yytext);
                    return NUM;
                }
{FLOAT}         {
                    yylval.val = atoi(yytext);
                    return NUM;
                }
{ASSIGNOP}      { return '='; }
{ARITHOP}       { return yytext[0]; }
{PAREN}         { return yytext[0]; }
{WHITESPACE}    /* strip whitespace */
\n              { return END_OF_LINE; }
 .              { printf("Unrecognized character: %s on line: %d\n",
                         yytext, yylineno); }

%%

