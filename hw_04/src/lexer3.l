%option warn nodefault
%option noinput nounput noyywrap yylineno
%option header-file="lexer3.h"
/* %option debug */

%top{
/* For fileno(3). See feature_test_macros(7) manpage. */
#define _GNU_SOURCE
}

%{
/* System headers */
#include <assert.h>
#include <stdio.h>

/* Custom headers */
/* #define YYSTYPE semrec_t* */
#include <y.tab.h>
#include <ourtypes.h>
#include <symtab.h>

/* Prototypes and C macro definitions */
#define MAX_ID_LENGTH 256
%}

letter   [A-Za-z]
digit    [0-9]
kwInt    "int"
kwFloat  "float"
kwVoid   "void"
kwIf     "if"
kwElse   "else"
kwWhile  "while"
kwFor	 "for"
kwStruct "struct"
kwUnion  "union"
kwTypedef "typedef"
kwReturn "return"
ID       ({letter})({letter}|{digit}|"_")*
op_assign        "="
op_or   "||"
op_and  "&&"
op_not  "!"
op_eq   "=="
op_ne   "!="
op_lt   "<"
op_gt   ">"
op_le   "<="
op_ge   ">="
op_plus "+"
op_minus "-"
op_times "*"
op_divide "/"
iconstant {digit}+
fconstant {digit}+(("."){digit}+)?
comment	"/*"(([^*])|([*]+[^/*]))*("*")+"/"
s-const \"([^"\n])*\"
ws       [ \t]+
newline  "\n"
mk_lparen "("
mk_rparen ")"
mk_lbrace "{"
mk_rbrace "}"
mk_lsqbrace "["
mk_rsqbrace "]"
mk_comma  ","
mk_semicolon ";"
mk_dot  "."
error    .

%%

{ws}            ;   /* do nothing with whitespace */
{comment}	;
{kwInt}         return INT;
{kwFloat}       return FLOAT;
{kwVoid}	return VOID;
{kwIf}          return IF;
{kwElse}        return ELSE;
{kwWhile}       return WHILE;
{kwFor}      	return FOR;
{kwStruct}	return STRUCT;
{kwUnion}	return UNION;
{kwTypedef}     return TYPEDEF;
{kwReturn}      return RETURN;
{ID}            {
                    int len = strlen (yytext);
                    assert (len < MAX_ID_LENGTH);
					printf("making ID with name = %s\n", yytext);
                    yylval.sem_ptr = new_semrec (yytext); 
                    return ID;
                }
{op_assign}     return OP_ASSIGN;
{op_and}        return OP_AND;
{op_or}         return OP_OR;
{op_not}        return OP_NOT;
{op_eq}         return OP_EQ;
{op_ne}         return OP_NE;
{op_lt}         return OP_LT;
{op_gt}         return OP_GT;
{op_le}         return OP_LE;
{op_ge}         return OP_GE;
{op_plus}       return OP_PLUS;
{op_minus}      return OP_MINUS;
{op_times}      return OP_TIMES;
{op_divide}     return OP_DIVIDE;
{iconstant}	{
                    semrec_t *p = new_semrec ("");
                    p->type = TYPE_INT;
                    p->is_const = TRUE;
                    p->value.intval = atoi (yytext);
                    yylval.sem_ptr = p;
                    return CONST;
                }
{fconstant}     {
                    semrec_t *p = new_semrec ("");
                    p->type = TYPE_FLOAT;
                    p->is_const = TRUE;
                    p->value.fval = atof (yytext);
                    yylval.sem_ptr = p;
                    return CONST;
                }
{s-const}       {
                    semrec_t *p = new_semrec ("");
                    p->type = TYPE_STRING;
                    p->is_const = TRUE;
                    p->value.stringval = malloc (sizeof (char) * strlen (yytext) + 1);
                    p->value.stringval = strcpy (p->value.stringval, yytext);
                    yylval.sem_ptr = p;
                    return CONST;
                }
{mk_lparen}     return MK_LPAREN;
{mk_rparen}     return MK_RPAREN;
{mk_lbrace}     return MK_LBRACE;
{mk_rbrace}     return MK_RBRACE;
{mk_lsqbrace}   return MK_LB;
{mk_rsqbrace}   return MK_RB;
{mk_comma}      return MK_COMMA;
{mk_semicolon}  return MK_SEMICOLON;
{mk_dot}	return MK_DOT;
{newline}       ; /* we use the yylineno feature */
{error}         return ERROR;

%%

