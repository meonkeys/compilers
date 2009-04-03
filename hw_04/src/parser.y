%{
/* System Headers */
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* Custom Headers */
#include <ourtypes.h>
#include <symtab.h>
#include <util.h>

int yylex (void);
void yyerror (char const *fmt, ...);
extern int yylineno;

#include <y.tab.h>

static char *ERR_START = "Error found in line";
%}

%defines
%error-verbose
%output="y.tab.c"

%union{
    struct semrec_s* sem_ptr;
}

/*
 * Expect 1 shift/reduce conflict. Bison automatically handles the "dangling
 * else" problem by choosing to shift when either shift or reduce is
 * applicable.
 */
%expect 1

%token <sem_ptr> ID
%token <sem_ptr> CONST
%token VOID
%token INT
%token FLOAT
%token IF
%token ELSE
%token WHILE
%token FOR
%token STRUCT
%token UNION
%token TYPEDEF
%token OP_ASSIGN
%token OP_OR
%token OP_AND
%token OP_NOT
%token OP_EQ
%token OP_NE
%token OP_GT
%token OP_LT
%token OP_GE
%token OP_LE
%token OP_PLUS
%token OP_MINUS
%token OP_TIMES
%token OP_DIVIDE
%token MK_LB
%token MK_RB
%token MK_LPAREN
%token MK_RPAREN
%token MK_LBRACE
%token MK_RBRACE
%token MK_COMMA
%token MK_SEMICOLON
%token MK_DOT
%token ERROR
%token RETURN

%left OP_OR OP_AND
%left OP_LT OP_GT OP_GE OP_LE OP_NE OP_EQ
%left OP_PLUS OP_MINUS
%left OP_TIMES OP_DIVIDE
%left MK_DOT

%type <sem_ptr> cexpr;
%type <sem_ptr> cfactor;
%type <sem_ptr> decl_list;
%type <sem_ptr> expr;
%type <sem_ptr> factor;
%type <sem_ptr> function_decl;
%type <sem_ptr> func_start;
%type <sem_ptr> init_id;
%type <sem_ptr> id_list;
%type <sem_ptr> init_id_list;
%type <sem_ptr> nonempty_relop_expr_list;
%type <sem_ptr> param;
%type <sem_ptr> param_list;
%type <sem_ptr> relop_expr_list;
%type <sem_ptr> relop_expr;
%type <sem_ptr> stmt;
%type <sem_ptr> struct_body;
%type <sem_ptr> struct_type;
%type <sem_ptr> type;
%type <sem_ptr> unary;
%type <sem_ptr> var_decl;
%type <sem_ptr> var_ref;

%start program

%%

program		: global_decl_list
		;

global_decl_list: global_decl_list global_decl
		| /* empty */
		;

global_decl	: decl_list function_decl
		| function_decl
		;

function_decl	: func_start MK_LPAREN param_list MK_RPAREN MK_LBRACE block MK_RBRACE
			{
				$1->value.funcval->param_list = $3;
				$1->value.funcval->num_params = list_length($3);
				putsym($1);
				$$ = $1;
			}
		| error MK_RBRACE { yyerrok }
		;

func_start	: type ID
			{
				$2->type = TYPE_FUNCTION;
				$2->value.funcval = malloc(sizeof(func_t));
				assert (NULL != $2->value.funcval);
				$2->value.funcval->return_type = $1->type;
				our_free($1);
				$$ = $2;
			}
		| VOID ID /* function returns void */
			{
				$2->type = TYPE_FUNCTION;
				$2->value.funcval = malloc(sizeof(func_t));
				assert (NULL != $2->value.funcval);
				$2->value.funcval->return_type = TYPE_VOID;
				/* putsym($2) */ /* add to symtab in function_decl */
				$$ = $2;
			}
		| ID ID   /* for a typedef'd return type */
			{
				$2->type = TYPE_FUNCTION;
				$2->value.funcval = malloc(sizeof(func_t));
				assert (NULL != $2->value.funcval);

				$$ = getsym($1->name);
				/* first ID not in symtab */
				if(NULL == $$){
					yyerror("%s %d: ID (%s) undeclared.", ERR_START, yylineno, $1->name);
					$1->is_temp = TRUE;
					our_free($1);
					YYERROR;
				}
				/* first ID found in symtab */
				else{
					$2->value.funcval->return_type = $1->type;
					/* Do not free first ID from symbol table */
					$$ = $2;
				}
			}
		;

param_list	: param_list MK_COMMA param
			{
				$1->next = $3;
				$$ = $1;
			}
		| param
		| /* empty */
			{
				/* so list_length() doesn't segfault */
				$$ = new_semrec ("--empty param list--")
			}
		;

param		: type ID
			{
				$2->type = $1->type;
				our_free($1);
				$$ = $2;
			}
		| struct_type ID
			{
				/* TODO: complete later */
				$$ = $1;
			}
		| type ID dim_fn
		| struct_type ID dim_fn
		;

dim_fn		: MK_LB expr_or_null MK_RB dimfn1
		;

dimfn1		: MK_LB expr MK_RB dimfn1
		| /* empty */
		;

expr_or_null	: expr
		| /* empty */
		;

block		: decl_list stmt_list
		| stmt_list
		| decl_list
		| /* empty */
		;

decl_list	: decl_list decl
		| decl
		;

decl		: type_decl
		| var_decl
		;

/* according to these rules, struct/union tag is _required_ */
type_decl	: TYPEDEF type id_list MK_SEMICOLON
			{
				putsymlist($3, $2->type); our_free($2);
			}
		| TYPEDEF ID id_list MK_SEMICOLON
		| TYPEDEF VOID id_list MK_SEMICOLON
		| struct_decl MK_SEMICOLON
		;

struct_decl	: struct_type id_list
	    	| struct_type ID id_list
	    	| struct_type ID struct_body
		| struct_type ID struct_body id_list
			{
				newstructlist($2->name, $3, $4, $1->type);
				our_free($1);
				our_free_list($4);

			}
		| struct_type struct_body id_list
			{
				newstructlist("", $2, $3, $1->type);
				our_free($1);
				our_free_list($3);

			}
		| TYPEDEF struct_type ID struct_body id_list
		| TYPEDEF struct_type struct_body id_list
		| TYPEDEF struct_type id_list
		;

struct_body	: MK_LBRACE decl_list MK_RBRACE
			{
				$$ = $2;
			}
		;

/* This production inserts sym_recs into the symbol table
 * No real need to keep them on the value stack
 */
var_decl	: type init_id_list MK_SEMICOLON
			{
				putsymlist ($2, $1->type); our_free($1);
			}
		| VOID id_list MK_SEMICOLON
			{
				yyerror("%s %d: Invalid variable type (%s).", ERR_START, yylineno, "void");
				our_free_list($2);
				YYERROR;
			}
		| type error MK_SEMICOLON { yyerrok } /* FIXME: Review. Is this correct? */
		| ID id_list MK_SEMICOLON
			{
				$$ = getsym($1->name);
				if(NULL == $$){
					yyerror("%s %d: ID (%s) undeclared.", ERR_START, yylineno, $1->name);
					$1->is_temp = TRUE;
					our_free($1);
					YYERROR;
				}
				else if(TYPE_STRUCT == $$->type || TYPE_UNION == $$->type){
					newstructlist($$->value.structval->tag, $$->value.structval->member_list, $2, $$->type);

				}
				else{
					putsymlist($2, $$->type);
				}
			}
		;

type		: INT
                        {
				$$ = new_semrec ("INT");
				$$->type = TYPE_INT;
				$$->is_temp = TRUE;
                        }
		| FLOAT
                        {
				$$ = new_semrec ("FLOAT");
				$$->type = TYPE_FLOAT;
				$$->is_temp = TRUE;
                        }
		;

struct_type	: STRUCT
			{
				$$ = new_semrec("STRUCT");
				$$->type = TYPE_STRUCT;
				$$->is_temp = TRUE;
			}
		| UNION
			{
				$$ = new_semrec("UNION");
				$$->type = TYPE_UNION;
				$$->is_temp = TRUE;
			}
		;

id_list		: ID
		| id_list MK_COMMA ID {$1->next = $3; $$ = $1;}
		| id_list MK_COMMA ID dim_decl
		| id_list MK_COMMA dim_decl ID { yyerror("%s %d: Dimensions must follow ID.", ERR_START, yylineno); YYERROR }
		| ID dim_decl
/*
		| id_list ID 
			{
				yyerror("%s %d: IDs must be seperated by commas.\n", ERR_START, yylineno);
				YYERROR;
			}
*/		
		;

dim_decl	: MK_LB cexpr MK_RB
		| MK_LB MK_RB { yyerror("%s %d: Empty dimensions disallowed.", ERR_START, yylineno); YYERROR }
		| dim_decl MK_LB cexpr MK_RB
		;

cexpr		: cexpr OP_PLUS cexpr
			{
				$$ = arith_op_type_reduce($1, $3);
				our_free($1);
				our_free($3);
			}
		| cexpr OP_MINUS cexpr
			{
				$$ = arith_op_type_reduce($1, $3);
				our_free($1);
				our_free($3);
			}
		| cexpr OP_TIMES cexpr
			{
				$$ = arith_op_type_reduce($1, $3);
				our_free($1);
				our_free($3);
			}
		| cexpr OP_DIVIDE cexpr
			{
				$$ = arith_op_type_reduce($1, $3);
				our_free($1);
				our_free($3);
			}
		| cfactor
		;

cfactor		: CONST
		| MK_LPAREN cexpr MK_RPAREN {$$ = $2 }
		| ID
			{
				yyerror("%s %d: Variable arrays are disallowed.", ERR_START, yylineno);
				YYERROR
			}
		;

init_id_list	: init_id
		| init_id_list MK_COMMA init_id {$1->next = $3; $$ = $1;}
		;

init_id		: ID
		| dim_decl ID { yyerror("%s %d: Dimensions must follow ID.", ERR_START, yylineno); YYERROR }
		| ID dim_decl
		| ID OP_ASSIGN relop_expr
		;

stmt_list	: stmt_list stmt
		| stmt
		;

stmt		: MK_LBRACE block MK_RBRACE
		| ID MK_LPAREN relop_expr_list MK_RPAREN MK_SEMICOLON
		| WHILE MK_LPAREN relop_expr_list MK_RPAREN stmt
		| FOR MK_LPAREN assign_expr_list MK_SEMICOLON relop_expr_list MK_SEMICOLON assign_expr_list MK_RPAREN stmt
		| var_ref OP_ASSIGN relop_expr MK_SEMICOLON
			{
				/*
				printf("$1=%p\t$3=%p\n", (void*)$1, (void*)$3);
				printf("$1 name = %s\n", $1->name);
				printf("$1 type = %d\n", $1->type);
				*/
				if(TRUE == typecmp($1->type, $3->type)){
					our_free($1); /* var_ref so it's temp */
					our_free($3);
					/* ID gets new value? */
				}
			}
		| IF MK_LPAREN relop_expr_list MK_RPAREN stmt if_stmt_tail
		| error MK_SEMICOLON { yyerrok }
		| MK_SEMICOLON
		| RETURN MK_SEMICOLON
		| RETURN relop_expr MK_SEMICOLON
		;

if_stmt_tail	: ELSE stmt
		| /* empty */
		;

assign_expr_list: nonempty_assign_expr_list
		| /* empty */
		;

nonempty_assign_expr_list: nonempty_assign_expr_list MK_COMMA assign_expr
		| assign_expr
/* FIXME: missing semicolon! */

/* I don't know why this isnt used, it's in stmt instead */
assign_expr	: ID OP_ASSIGN relop_expr
		| relop_expr
/* FIXME: missing semicolon! */


relop_expr	: expr
		| relop_expr rel_op expr
		;

rel_op		: OP_LT
		| OP_GT
		| OP_GE
		| OP_LE
		| OP_NE
		| OP_EQ
		| OP_OR
		| OP_AND
		;

relop_expr_list	: nonempty_relop_expr_list
		| /* empty */ {}
		;

nonempty_relop_expr_list: nonempty_relop_expr_list MK_COMMA relop_expr
			{
				$1->next = $3;
				$$ = $1;
			}
		| relop_expr
		;

expr		: expr OP_PLUS expr
			{
				$$ = arith_op_type_reduce($1, $3);
				our_free($1);
				our_free($3);
			}
		| expr OP_MINUS expr
			{
				$$ = arith_op_type_reduce($1, $3);
				our_free($1);
				our_free($3);
			}
		| expr OP_TIMES expr
			{
				$$ = arith_op_type_reduce($1, $3);
				our_free($1);
				our_free($3);
			}
		| expr OP_DIVIDE expr
			{
				$$ = arith_op_type_reduce($1, $3);
				our_free($1);
				our_free($3);
			}
		| unary
		;

unary		: OP_MINUS unary {$$ = $2}
		| OP_NOT unary {$$ = $2}
		| factor
		;

factor		: MK_LPAREN relop_expr MK_RPAREN
		| CONST
		| ID MK_LPAREN relop_expr_list MK_RPAREN
			/* Function call */
			{
				$$ = getsym($1->name);
				/* is the function in the symbol table? */
				if(NULL == $$){
					yyerror("%s %d: ID (%s) undeclared.", ERR_START, yylineno, $1->name);
					$1->is_temp = TRUE;
					our_free($1);
					YYERROR;
				}
				/* is the param list length correct? */

				if(NULL == $$->value.funcval){
					yyerror("%s %d: Function (%s) undeclared.", ERR_START, yylineno, $$->name);
					YYERROR;
				}

				if(list_length($3) > $$->value.funcval->num_params){
					yyerror("%s %d: too many arguments to function (%s).", ERR_START, yylineno, $1->name);
					YYERROR;
				}
				else if (list_length($3) < $$->value.funcval->num_params){
					yyerror("%s %d: too few arguments to function (%s).", ERR_START, yylineno, $1->name);
					YYERROR;
				}
				$$ = $1; /* return the function call semrec */
			}
		| var_ref
		;

var_ref		: ID
			{
				$$ = getsym($1->name);
				/* Gotta do this to delete dangling semrec_ts if
				      they already exist in the symbol table */
				if(NULL == $$){
					$$ = $1;
					yyerror("%s %d: ID (%s) undeclared.", ERR_START, yylineno, $1->name);
					YYERROR;
					/*putsym($1);*/
				}
				$1->type = $$->type;
				$1->is_temp = TRUE;
				our_free($1);
			}
		| var_ref dim
		| var_ref struct_tail
		;


dim		: MK_LB expr MK_RB
		;

struct_tail	: MK_DOT ID
		;

%%

/*
vim: noexpandtab shiftwidth=8 tabstop=8
*/
