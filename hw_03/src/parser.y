%{
#include <y.tab.h>

int yylex(void);
void yyerror (char const *mesg);
%}

%defines
%output="y.tab.c"

/*
 * Expect 1 shift/reduce conflict. Bison automatically handles the "dangling
 * else" problem by choosing to shift when either shift or reduce is
 * applicable.
 */
//%expect 1

%token ID
%token CONST
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
		| func_start MK_LPAREN param_list error MK_LBRACE block MK_RBRACE
		;

func_start      : type ID
		| VOID ID /* function returns void */
		| ID ID   /* for a typedef'd return type */
		;

param_list	: param_list MK_COMMA param
		| param
		| /* empty */
		;

param		: type ID
		| struct_type ID
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
		| TYPEDEF ID id_list MK_SEMICOLON
		| TYPEDEF VOID id_list MK_SEMICOLON
		| struct_decl MK_SEMICOLON
		;

struct_decl	: struct_type struct_body
			/*FIXME: whats the point of this produciton? */
			| struct_type ID
			| struct_type ID id_list 
			| struct_type ID struct_body
			| struct_type ID struct_body id_list
			| struct_type struct_body id_list
			| TYPEDEF struct_type ID struct_body ID
			| TYPEDEF struct_type struct_body ID
			| TYPEDEF struct_type id_list
			/* no tag or name: error */
			| struct_type MK_LBRACE decl_list MK_RBRACE error
		;

struct_body : MK_LBRACE decl_list MK_RBRACE
			;

var_decl	: type init_id_list MK_SEMICOLON
		| struct_type id_list MK_SEMICOLON
		| ID id_list MK_SEMICOLON
		;

type		: INT
		| FLOAT
		;

struct_type	: STRUCT 
            | UNION
		;

id_list		: ID
		| id_list MK_COMMA ID
		| id_list MK_COMMA ID dim_decl
		| ID dim_decl
		;

dim_decl	: MK_LB cexpr MK_RB
		| dim_decl MK_LB cexpr MK_RB
		;

cexpr		: cexpr add_op mcexpr
		| mcexpr
		;

mcexpr		: mcexpr mul_op cfactor
		| cfactor
		;

cfactor		: CONST
		| MK_LPAREN cexpr MK_RPAREN
		;

init_id_list	: init_id
		| init_id_list MK_COMMA init_id
		;

init_id		: ID
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
		| IF MK_LPAREN relop_expr_list MK_RPAREN stmt if_stmt_tail
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

assign_expr	: ID OP_ASSIGN relop_expr
		| relop_expr


relop_expr	: relop_term
		| relop_expr OP_OR relop_term
		;

relop_term	: relop_factor
		| relop_term OP_AND relop_factor
		;

relop_factor	: expr
		| expr rel_op expr
		;

rel_op		: OP_LT
		| OP_GT
		| OP_GE
		| OP_LE
		| OP_NE
		| OP_EQ
		;

relop_expr_list	: nonempty_relop_expr_list
		| /* empty */
		;

nonempty_relop_expr_list: nonempty_relop_expr_list MK_COMMA relop_expr
		| relop_expr
		;

expr		: expr add_op term
		| term
		;

add_op		: OP_PLUS
		| OP_MINUS
		;

term		: term mul_op unary
		| unary
		;

mul_op		: OP_TIMES
		| OP_DIVIDE
		;

unary		: OP_MINUS unary
		| OP_NOT unary
		| factor
		;

factor		: MK_LPAREN relop_expr MK_RPAREN
		| OP_NOT MK_LPAREN relop_expr MK_RPAREN
		| CONST
		| OP_NOT CONST
		| ID MK_LPAREN relop_expr_list MK_RPAREN
		| OP_NOT ID MK_LPAREN relop_expr_list MK_RPAREN
		| var_ref
		| OP_NOT var_ref
		;

var_ref		: ID
		| var_ref dim
		| var_ref struct_tail
		;


dim		: MK_LB expr MK_RB
		;

struct_tail	: MK_DOT ID
		;

%%

