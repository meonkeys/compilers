/* ===== Definition Section ===== */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <header.h>

int yylex (void);
void yyerror (char const *fmt, ...);

#include <y.tab.h>

int linenumber = 1;
int scope=0;
int STRUCT_DEC=0;
int FUNC=0;
TYPE func_return = -1;
int STRUCT_P = 0;
int IS_RETURN=0;
int GLOBAL_ERROR = 0;
int ISERROR=0;
void VerifyMainCall();

/* Our additions */
int offset = -4;
int param_offset = 8;
char* frame_data = NULL;

int GLOBAL_DECLS_STARTED=0;
/* FIXME: this is only a temporary way to handle unique label creation for
while and if-else control structures (and for loops, in the next assignment).
We'll need a stack to handle nested control structures (see 5161Class16.pdf).
*/
int LABEL_NUM=0;
%}

%defines
%expect 1
%error-verbose
%output="y.tab.c"

%union{
	char *lexeme;
	struct CON_Type_s *const1;
	int intval;
	int label_num;
	int Type;
	char * xst;
	struct MyArrInfo *arr_info;
	struct id_list *P_id_l;
	struct var_decl_s *P_var_d;
	struct init_id_s *P_ini_i;
	struct def_list *P_def_l;
	struct TypeList * P_typ_l;
	struct var_ref_s * P_var_r;
	struct param_s *P_par;
	struct param_list *plist;
};


%token <lexeme>ID
%token <const1>CONST
%token VOID
%token INT
%token FLOAT
%token IF
%token ELSE
%token WHILE
%token FOR
%token STRUCT
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


%type <xst> struct_tail tag  opt_tag struct_type
%type <P_typ_l> relop_expr_list nonempty_relop_expr_list
%type <intval> mul_op add_op rel_op type cfactor mcexpr cexpr
%type <Type> nonempty_assign_expr_list  assign_expr_list  stmt  stmt_list type_decl  decl  decl_list  block  function_decl  global_decl  global_decl_list  program
%type <P_id_l> id_list  init_id_list
%type <arr_info> dim_decl dim_fn dimfn1
%type <P_var_d> var_decl def
%type <P_ini_i> init_id
%type <P_var_r> var_ref  dim factor term expr relop_factor relop_term relop_expr assign_expr test expr_null
/*except NT var_ref  the other NT's do not need the name field but the same structure is used*/
%type <P_par> param
%type <P_def_l> def_list
%type <plist> param_list

%start program

%%

/* ==== Grammar Section ==== */

/* Productions */               /* Semantic actions */
program		: global_decl_list {
			if ($1==ERROR_ || GLOBAL_ERROR) {
				printf("error:  Semantic Analysis failed due to errors\n");
				yyerror("");
			} else
				VerifyMainCall();
			}
		;

global_decl_list: global_decl_list global_decl{$$=($2==ERROR_)?ERROR_:$1;}
                |{$$=ZERO_;}
		;

global_decl	: decl_list function_decl{$$=(($1==ERROR_)||($2==ERROR_))?ERROR_:ZERO_;}
		| function_decl{$$=$1;}
		;

function_decl	: type ID MK_LPAREN {scope++;IS_RETURN=0;} param_list {$<Type>$=func_enter_ST($1,$2,$5);func_return=$1; } MK_RPAREN {gen_prologue($2)} MK_LBRACE block MK_RBRACE {delete_scope(scope);scope--;$$=((check_return(IS_RETURN,$1)==ERROR_)||($<Type>6==ERROR_)||($10==ERROR_))?ERROR_:ZERO_; gen_epilogue($2); offset = -4;}

		| struct_type ID MK_LPAREN {printf("error %d: functions do not return structs in C--\n",linenumber);scope++;IS_RETURN=0;} param_list {func_enter_ST(STR_,$2,$5);func_return=ERROR_; }MK_RPAREN MK_LBRACE block MK_RBRACE{delete_scope(scope);scope--;$$=ERROR_; offset = -4;}

		| VOID ID MK_LPAREN {scope++;IS_RETURN=0;} param_list {$<Type>$=func_enter_ST(VOID_,$2,$5);func_return=VOID_; ;}MK_RPAREN {gen_prologue($2)} MK_LBRACE block MK_RBRACE{delete_scope(scope);scope--;$$=(($<Type>6==ERROR_)||($10==ERROR_))?ERROR_:ZERO_; gen_epilogue($2); offset = -4;}

		| type ID MK_LPAREN   MK_RPAREN {$<Type>$=func_enter_ST($1,$2,NULL);func_return=$1; gen_prologue($2);}MK_LBRACE{scope++;IS_RETURN=0;} block MK_RBRACE{delete_scope(scope);scope--;$$=((check_return(IS_RETURN,$1)==ERROR_)||($<Type>5==ERROR_)||($8==ERROR_))?ERROR_:ZERO_; gen_epilogue($2); offset = -4;}

		| struct_type ID MK_LPAREN  MK_RPAREN MK_LBRACE {printf("error %d: functions do not return structs in C--\n",linenumber);scope++;IS_RETURN=0;func_enter_ST(STR_,$2,NULL);func_return=ERROR_;} block MK_RBRACE{delete_scope(scope);scope--;$$=ERROR_; offset = -4;}

		| VOID ID MK_LPAREN MK_RPAREN {gen_prologue($2)} MK_LBRACE {scope++;IS_RETURN=0;func_enter_ST(VOID_,$2,NULL);func_return=VOID_;} block MK_RBRACE {delete_scope(scope);scope--;$$=(($<Type>6==ERROR_)||($8==ERROR_))?ERROR_:ZERO_; gen_epilogue($2); offset = -4;}
		;

param_list	: param_list MK_COMMA  param{$$=MakeParamList($1,$3);}
		| param	{$$=MakeParamList(NULL,$1);}
		;

param		: type ID{
			param* PP;
			PP=Allocate(PARAM);
			PP->name = $2;
			PP->type=$1;
			if(param_P(PP,$2)==ERROR_)
				$$=NULL;
			else
				$$=PP;
		}
		| struct_type ID{
			printf("error %d: struct can not be passed as parameter\n",linenumber);
			$$=NULL;
		}
		| type ID dim_fn{
			if($3==NULL)
				$$=NULL;
			else{
				param* PP;
				PP=Allocate(PARAM);
				PP->type=ARR_;
				PP->arrtype=$1;
				PP->dim=$3->dim;
				if(param_P(PP,$2)==ERROR_)
					$$=NULL;
				else
					$$=PP;
			}
		}
		| struct_type ID dim_fn {
			printf("error %d: struct can not be passed as parameter\n",linenumber);
			$$=NULL;
		}
		;
dim_fn		:MK_LB expr_null MK_RB dimfn1{
			if($2!=NULL&&$2->type!=INT_){
				printf("error %d: non integer type in dimension declaration\n",linenumber);
				$$=NULL;
			}
			else
			{
			$4->dim = $4->dim+1;
			$$ = $4; }
		}
		;
dimfn1		:MK_LB expr MK_RB dimfn1{
			if($2->type!=INT_){
				printf("error %d: non integer type in dimension declaration\n",linenumber);
				$$=NULL;
			}
			else{
			$4->dim = $4->dim+1;
			$$ = $4; }
		}
		|
			{
			ArrayInfo *temp;
			temp = (ArrayInfo*)malloc(sizeof(ArrayInfo));
			temp->dim = 0;
			$$ = temp; }
		;
expr_null	:expr{$$=$1;}
		|{$$=NULL;}
		;

block           : decl_list stmt_list {$$=(($1==ERROR_)||($2==ERROR_))?ERROR_:ZERO_;}
                | stmt_list
                | decl_list{$$=$1;}
                |{$$=ZERO_;}
                ;

decl_list	: decl_list decl{if($2==ERROR_)$$=$2;else $$=$1;}
		| decl{$$=$1;}
		;

decl		: type_decl {$$=$1;}
		| var_decl {
			$$=decl_enter_ST($1);
			if (scope == 0) {
				if (!GLOBAL_DECLS_STARTED) {
					asm_emit_global_decls_start ();
					GLOBAL_DECLS_STARTED = 1;
				}
				asm_emit_global_decl_list($1);
			}else{
				asm_emit_scoped_decl_list($1);
			}
		}
		;

type_decl 	: TYPEDEF type id_list MK_SEMICOLON{
			$$=type_decl_enter_ST1($2,$3);

		}
		| TYPEDEF VOID id_list MK_SEMICOLON{$$=ZERO_;}
		| TYPEDEF struct_type id_list MK_SEMICOLON{
			if(if_user_name($2)){
				printf("error %d: cannot have a tag in typedef struct \n",linenumber);
				$$=ERROR_;
			}
			else
				$$=ZERO_;
			type_decl_enter_ST2($2,$3);
		}
		| struct_type MK_SEMICOLON{
				printf("error %d: struct declaration with no variables\n",linenumber);
				$$=ERROR_;
		}
		;

var_decl	: type init_id_list MK_SEMICOLON{
			$$=Allocate(VAR_DECL);
			$$->linnum=linenumber;
			$$->P_id_l = reverse_id_list($2); /* we need to reverse the list because it's built backwards */
			$$->type=$1;
			$$->type_name=NULL;
			if(scope > 0){
				offset = set_var_decl_list_offsets($$, offset);
			}
		}
		| struct_type id_list MK_SEMICOLON{
			$$=Allocate(VAR_DECL);
			/* symtab *STP; */
			if($1==NULL)
				$$->type=ERROR_;
			else{
				$$->linnum=linenumber;
				$$->P_id_l=$2;
				$$->type=STR_VAR_;
				if(STRUCT_P==1)
					$$->type_name=$1;
				else{
					printf("error %d: cannot have `struct tag' declarations\n",linenumber);
					$$->type=ERROR_;
				}
			}
		}
		| ID id_list MK_SEMICOLON {
			symtab *STP;
			init_id * PII;
			id_list * PIL;
			char *name;
			$$=Allocate(VAR_DECL);
			if((STP=lookup($1))==NULL){
				printf("error %d: undeclared type %s\n",linenumber,$1);
				$$->type=ERROR_;
				$$->P_id_l=$2;
				$$->linnum=linenumber;
			}
			else if(STP->type!=TYPEDEF_){
				printf("error %d: %s not a type\n",linenumber,$1);
				$$->type=ERROR_;
				$$->P_id_l=$2;
				$$->linnum=linenumber;
			}
			else{
				switch(STP->type_when_def){
					case TYPEDEF_INT:
						$$->type=INT_;
						$$->type_name=NULL;
						break;
					case TYPEDEF_FLT:
						$$->type=FLOAT_;
						$$->type_name=NULL;
						break;
					case TYPEDEF_ARR:
						PIL=$2;
						$$->type=STP->symtab_u.st_arr->arrtype;
						if(STP->symtab_u.st_arr->arrtype==STR_)
							$$->type_name=STP->symtab_u.st_arr->type_name;
						do{
							PII=$2->P_ini_i;
							switch(PII->type){
								case ARR_:

									PII->init_id_u.P_arr_s->arr_info->dim+=STP->symtab_u.st_arr->dim;
									break;
								default:

									PII->type=ARR_;
									name=PII->init_id_u.name;
									PII->init_id_u.P_arr_s=Allocate(ARRAY_SEM);
									PII->init_id_u.P_arr_s->arr_info=Allocate(TYPE_ARR);
									PII->init_id_u.P_arr_s->arr_info->dim=STP->symtab_u.st_arr->dim;
									PII->init_id_u.P_arr_s->name=name;
									break;
							}

						}while((PIL=PIL->next));
						break;
					case TYPEDEF_STR:
						$$->type=STR_VAR_;
						$$->type_name=STP->symtab_u.type_name;
						break;
				}
				$$->linnum=linenumber;
				$$->P_id_l=$2;
			}
		}

		;

type		: INT{$$=INT_;}
		| FLOAT{$$=FLOAT_;}
		;

struct_type	: STRUCT opt_tag MK_LBRACE {STRUCT_DEC++;} def_list {STRUCT_DEC--;}MK_RBRACE{
			if(!struct_type_P($2,$5))
				$$=NULL;
			else
				$$=$2;
			STRUCT_P=1;
		}
		| STRUCT tag{
			$$=$2;
			STRUCT_P=2;
		}
		;

def_list	: def_list def{$$=def_list_P($1,$2);}
		|{$$=NULL;}
		;

def		: var_decl {$$=$1;}
		;

opt_tag		: tag
		{	if($1!=NULL)
				printf("error %d: tags are not supported by C--\n",linenumber);
			$$=get_temp_name();
		}
		| {$$=get_temp_name();}/* empty */

tag		: ID{$$=$1;}


id_list		: ID{$$=Create_Id_List($1);}
		| id_list MK_COMMA ID{$$=Merge_Id_List($1,$3);}
		| id_list MK_COMMA ID dim_decl{$$=Id_List_Array($1,$3,$4);}
		| ID dim_decl{$$=Id_List_Array(NULL,$1,$2);}
		;
dim_decl	: MK_LB cexpr MK_RB
			{
			ArrayInfo* temp;
			if ($2 <= 0) {
				printf("Invalid dimension declaration %d \n", $2);
				yyerror("");
			} else {
			temp = (ArrayInfo*)malloc(sizeof(ArrayInfo));
			temp->dim = 1;
			temp->dim_limit[0] = $2;
			$$ = temp; }
			}
		| dim_decl MK_LB cexpr MK_RB
			{
			 if ($1->dim == 10 || ($3 <= 0) )
			 { /* Limit reached */
			 printf("Invalid dimension declaration \n");
			 yyerror("");
			 }
			 else {
			 $1->dim_limit[$1->dim] = $3;
			 $1->dim +=1;
			 $$ = $1;
			 }
			 }
		;
cexpr		: cexpr OP_PLUS mcexpr{ $$=$1+$3;}
		| cexpr OP_MINUS mcexpr {$$=$1-$3;}
		| mcexpr{$$=$1;}
		;
mcexpr		: mcexpr OP_TIMES cfactor {$$=$1*$3; }
		| mcexpr OP_DIVIDE cfactor {
			if ($3 == 0) printf("ERROR\n");
			if (($1 % $3) != 0) printf("ERROR\n");
			$$=$1/$3;
			}
		| cfactor {$$=$1;}
		;
cfactor:	CONST{
			if($1->const_type!=INTEGERC){
				printf("error %d: non integer expression in array dimension\n",linenumber);
				$$=ERROR_;
			}
			else
			{
				$$=$1->const_u.intval;
				}
		}
		|MK_LPAREN cexpr MK_RPAREN{$$=$2;}
		;

init_id_list	: init_id{
			$$=Allocate(ID_LIST);
			$$->P_ini_i=$1;
			$$->next=NULL;
		}
		| init_id_list MK_COMMA init_id{
			/*append at the beginging of list, need not preserve order*/
			id_list * PIL;
			PIL=Allocate(ID_LIST);
			PIL->next=$1;
			PIL->P_ini_i=$3;
			$$=PIL;
		}
		;

init_id		: ID{
			$$=Allocate(INIT_ID);
			$$->assignment_during_initialization=0;
			$$->init_id_u.name=$1;
			$$->type=ZERO_;		/*type is not known at this point
								so we give non error type here.
								should be checked above.*/
		}
		| ID dim_decl{
		        int j;
			$$=Allocate(INIT_ID);
			$$->type=ARR_;			/*we know this is array decl*/
			$$->init_id_u.P_arr_s=Allocate(ARRAY_SEM);
			$$->init_id_u.P_arr_s->arr_info=Allocate(TYPE_ARR);
			$$->init_id_u.P_arr_s->name=$1;
			$$->init_id_u.P_arr_s->arr_info->dim=$2->dim;
			for (j=0; j< 10; j++)
			{
				$$->init_id_u.P_arr_s->arr_info->dim_limit[j] = $2->dim_limit[j];
			}
		}

		/* assignment during variable initialization */
		| ID OP_ASSIGN relop_expr{
			$$=Allocate(INIT_ID);
			$$->assignment_during_initialization=1;
			if(STRUCT_DEC){
				printf("error %d:assignment to variable %s in struct definition\n",linenumber,$1);
				$$->type=ERROR_;
			}
			else{
				if($3->type!=INT_&&$3->type!=FLOAT_){
					printf("error %d: initialization of variable %s with expression of type %s\n",linenumber,$1,printtype($3->type));
					$$->type=ERROR_;
				}
				else { /* handle int or float (only) */
					$$->type=$3->type;
					if ($$->type == INT_){
						$$->val_u.intval = $3->tmp_val_u.tmp_intval;
					}
					else /* must be float */{
						$$->val_u.fval = $3->tmp_val_u.tmp_fval;
					}
				}
			}
			$$->init_id_u.name=$1;
		}
		;

stmt_list	: stmt_list stmt{
			if($2==ERROR_)
				$$=ERROR_;
			else
				$$=$1;
		}
		| stmt{$$=$1;}
		;

stmt		: MK_LBRACE {scope++;}block {delete_scope(scope);scope--;}MK_RBRACE{$$=$3;}
		| WHILE MK_LPAREN test {
			if(($3->type!=INT_)&&($3->type!=FLOAT_)){
				printf("error %d: condition not a basic type in while statement\n",linenumber);
			}
		}MK_RPAREN stmt{
			if($6==ERROR_) {
				$$=$6;
			} else {
				gen_control_iterate($3->label_num);
				gen_control_endlabel($3->label_num);
			}
		}
	        | FOR MK_LPAREN assign_expr_list MK_SEMICOLON relop_expr_list MK_SEMICOLON assign_expr_list MK_RPAREN stmt
		{
			if($3==ERROR_)
				$$=ERROR_;
			else if(check_relop_ex_lst($5)==ERROR_)
				$$=ERROR_;
			else if($7==ERROR_)
				$$=ERROR_;
			else
				$$=$7;
		}

		/* simple assignment statement */
		| var_ref OP_ASSIGN relop_expr MK_SEMICOLON{$$=stmt_assign_ex($1,$3);}
		| IF MK_LPAREN test MK_RPAREN stmt{
			if(($3->type!=INT_)&&($3->type!=FLOAT_)){
				printf("error %d: condition not a basic type in if statement\n",linenumber);
				$$=ERROR_;
			} else {
				$$=$5;
				gen_control_endlabel ($3->label_num);
			}
		}
		| IF MK_LPAREN test MK_RPAREN stmt ELSE
		{
			gen_control_exit_ifelse ($3->label_num);
			gen_control_endlabel ($3->label_num)
		}
		stmt
		{
			if(($3->type!=INT_)&&($3->type!=FLOAT_)){
				printf("error %d: condition not a basic type in if statement\n",linenumber);
				$$=ERROR_;
			} else {
				if (($5==ERROR_)||($8==ERROR_)) {
					$$=ERROR_;
				} else {
					$$=ZERO_;
				}
				gen_control_ifelse_endlabel ($3->label_num);
			}
		}

		/* function call */
		|ID MK_LPAREN relop_expr_list MK_RPAREN MK_SEMICOLON{
			var_ref *PVR;
			if ((strcmp($1,"write") == 0) ||
			   (strcmp($1,"WRITE") == 0)) {
				if($3==NULL){
					printf("error %d: too few arguments to function write\n",linenumber);
					$$=ERROR_;
				}
				if ($3->next !=NULL) {
					printf("error %d: too many arguments to function write\n",linenumber);
					$$=ERROR_;
				}
				else if($3->P_var_r->type!=INT_
				&& $3->P_var_r->type!=FLOAT_
				&& $3->P_var_r->type!=STRING_){
					printf("error %d: wrong type of argument %s to write\n",linenumber,printtype($3->P_var_r->type));
					$$=ERROR_;
				}
				else{
					asm_emit_write($3);
					$$=ZERO_;
				}
			}
			else {
			PVR=check_function($1,$3);
			/* TODO: save caller registers $t range iirc */
			asm_out("\tjal\t%s\n", $1);
			$$=PVR->type;
			}
		}
		| MK_SEMICOLON{$$=ZERO_;}
		| RETURN MK_SEMICOLON{
			if((func_return!=ERROR_)&&(func_return!=VOID_)){
				printf("error %d: returning nothing from function expecting to return %s\n",linenumber,printtype(func_return));
				$$=ERROR_;
			}
			else
				$$=ZERO_;
			IS_RETURN=1;
		}
		| RETURN relop_expr MK_SEMICOLON{
			if((func_return==ERROR_)||($2->type==ERROR_))
				$$=ZERO_;
			else if(func_return!=$2->type){
				switch (func_return){
					case INT_:
					case FLOAT_:
						if($2->type!=INT_&&$2->type!=FLOAT_){
							printf("error %d: incompatible return type, expects to return %s, returning %s\n",linenumber,printtype(func_return),printtype($2->type));
							$$=ERROR_;
						}
						break;
					case VOID_:
						printf("error %d: cannot return value from a void function\n",linenumber);
						break;
					default:
						printf("error %d: UNEXPECTED OUTCOME\n",linenumber);
						break;
				}
			}
			else
				$$=ZERO_;
			IS_RETURN=1;
		}
		;

assign_expr_list : nonempty_assign_expr_list{$$=$1;}
                |{$$=ZERO_;}
                ;

nonempty_assign_expr_list        : nonempty_assign_expr_list MK_COMMA assign_expr{
				if($3->type==ERROR_)
					$$=$3->type;
				else
					$$=$1;
			}

                | assign_expr{$$=$1->type;}
		;

test            : {
			$<label_num>$=++LABEL_NUM;
			gen_control_start ($<label_num>$);
		} assign_expr {
			$$=$2;
			if (0 == $$->place)
			{
				$$->place = asm_emit_relop_factor($2, NULL, 0);
			}
			gen_control_test ($2, $<label_num>1);
			$$->label_num = $<label_num>1;
		}
                ;

		/* assignment in initializer of "for loop" control expression */
assign_expr     : ID OP_ASSIGN relop_expr {$$=assign_ex($1,$3);}
                | relop_expr{$$=$1;}


relop_expr	: relop_term{$$=$1;}
		| relop_expr OP_OR relop_term{ $$=relop_extm($1,OP_OR,$3);}
		;

relop_term	: relop_factor{$$=$1;}
		| relop_term OP_AND relop_factor{ $$=relop_extm($1,OP_AND,$3);}
		;

relop_factor	: expr{$$=$1;}
		| expr rel_op expr{
			if(($1->type==ERROR_)||($3->type==ERROR_))
				$1->type=ERROR_;
			else if((($1->type!=INT_)&&($1->type!=FLOAT_))||(($3->type!=INT_)&&($3->type!=FLOAT_))){
				printf("error %d: relational operators applied to expression of non basic type\n",linenumber);
				$1->type=ERROR_;
			}
			else{
				$1->type=INT_;
				$1->place = asm_emit_relop_factor($1, $3, $2);
			}
			$$=$1;
		}
		;

rel_op		: OP_EQ{$$=OP_EQ;}
		| OP_GE{$$=OP_GE;}
		| OP_LE{$$=OP_LE;}
		| OP_NE{$$=OP_NE;}
		| OP_GT{$$=OP_GT;}
		| OP_LT{$$=OP_LT;}
		;

relop_expr_list	: nonempty_relop_expr_list {$$=$1;}
		| {$$=NULL;}
		;

nonempty_relop_expr_list	: nonempty_relop_expr_list MK_COMMA relop_expr{
					TypeList * TLP;
					TLP=$1;
					while(TLP->next!=NULL)
						TLP=TLP->next;
					TLP->next=Allocate(TYPELIST);
					TLP->next->next=NULL;
					TLP->next->P_var_r=$3;
					$$=$1;
				}
		| relop_expr{
			$$=Allocate(TYPELIST);
			$$->P_var_r=$1;
			$$->next=NULL;
		}
		;

expr		: expr add_op term{
			if($1->type==ERROR_||$3->type==ERROR_)
				$1->type=ERROR_;
			else if(($1->type!=INT_)&&($1->type!=FLOAT_)){
				printf("error %d: operator %s applied to non basic expr (%s)\n",linenumber,($2==OP_PLUS)?"+":"-",$1->name);
				$1->type=ERROR_;
			}
			else if(($3->type!=INT_)&&($3->type!=FLOAT_)){
				printf("error %d: operator %s applied to non basic factor (%s)\n",linenumber,($2==OP_TIMES)?"*":"/",$3->name);
				$1->type=ERROR_;
			}
			else{
				$1->type=(($1->type==FLOAT_)||($3->type==FLOAT_))?FLOAT_:INT_;
				$1->place = asm_emit_expr($1, $3, $2);
			}
			$$=$1;
			$$->name=NULL;
		}
		| term{$$=$1;}
		;

add_op		: OP_PLUS{$$=OP_PLUS;}
		| OP_MINUS{$$=OP_MINUS;}
		;

term		: term mul_op factor{
			if($1->type==ERROR_||$3->type==ERROR_)
				$1->type=ERROR_;
			else if(($1->type!=INT_)&&($1->type!=FLOAT_)){
				printf("error %d: operator %s applied to non basic term (%s)\n",linenumber,($2==OP_TIMES)?"*":"/",$1->name);
				$1->type=ERROR_;
			}
			else if(($3->type!=INT_)&&($3->type!=FLOAT_)){
				printf("error %d: operator %s applied to non basic factor (%s)\n",linenumber,($2==OP_TIMES)?"*":"/",$3->name);
				$1->type=ERROR_;
			}
			else{
				$1->type=($1->type==FLOAT_||$3->type==FLOAT_)?FLOAT_:INT_;
				$1->place = asm_emit_term($1, $3, $2);
			}
			$1->name=NULL;
			$$=$1;
		}
		| factor {$$=$1;}
		;

mul_op		: OP_TIMES{$$=OP_TIMES;}
		| OP_DIVIDE{$$=OP_DIVIDE;}
		;

factor		: MK_LPAREN relop_expr MK_RPAREN{$$=$2;}
		| OP_MINUS MK_LPAREN relop_expr MK_RPAREN{
			if($3->type!=INT_&&$3->type!=FLOAT_){
				printf("error %d: operator Unary Minus applied to expression of non basic type\n",linenumber);
			$3->type=ERROR_;
			}
			$$=$3;
		}
		| OP_NOT MK_LPAREN relop_expr MK_RPAREN{
			if($3->type!=INT_){
				if($3->type!=FLOAT_){
					printf("error %d: operator ! applied to non scalar expression\n",linenumber);

					$3->type=ERROR_;
				}
				else{
					printf("warning %d: operator ! applied to a float expression\n",linenumber);
					$3->type=INT_;
				}
			}
			$$=$3;
		}
		| CONST{
			$$=Allocate(VAR_REF);
			if($1->const_type==INTEGERC) {
				$$->type=INT_;
				$$->tmp_val_u.tmp_intval=$1->const_u.intval;
			}
			else if($1->const_type==FLOATC) {
				$$->type=FLOAT_;
				$$->tmp_val_u.tmp_fval=$1->const_u.fval;
			}
			else if($1->const_type==STRINGC){
				$$->type=STRING_;
				$$->tmp_val_u.tmp_str = $1->const_u.sc;
			}
			else
				$$->type=ERROR_;
			$$->name=NULL;
		}
		| OP_MINUS CONST{
			$$=Allocate(VAR_REF);
			if($2->const_type!=INTEGERC && $2->const_type!=FLOATC){
				printf("error %d: operator Unary Minus applied to non Basic constant\n",linenumber);
				$$->type=ERROR_;
			} else {
				if ($2->const_type==INTEGERC) {
					$$->type = INT_;
					$$->tmp_val_u.tmp_intval = -($2->const_u.intval);
				} else {
					$$->type = FLOAT_;
					$$->tmp_val_u.tmp_fval = -($2->const_u.fval);
				}
				$$->name=NULL;
			}
		}
		| OP_NOT CONST{
			$$=Allocate(VAR_REF);
			if($2->const_type!=INTEGERC){
				if($2->const_type!=FLOATC){
					printf("error %d: operator ! applied to non scalar constant\n",linenumber);
					$$->type=ERROR_;
				}else{
					printf("warning %d: operator ! applied to a float constant\n",linenumber);
					$$->type=INT_;
				}
			}
			else
				$$->type=INT_;
			$$->name=NULL;
		}
		| /* FUNCTION CALL */
			ID MK_LPAREN relop_expr_list MK_RPAREN{
			$$=check_function($1,$3);
			if(0 != strcmp($1, "write") && 0 != strcmp($1, "read") && 0 != strcmp($1, "fread")){
				asm_out("\tjal\t%s\n", $1);
			}
			
		}
		| OP_MINUS ID MK_LPAREN relop_expr_list MK_RPAREN{
			$$=check_function($2,$4);
			if(($$->type!=ERROR_)&&(($$->type!=INT_)||($$->type!=FLOAT_))){
				printf("error %d: unary minus applied to non scalar expression\n",linenumber);
				$$->type=ERROR_;
			}
		}
		| OP_NOT ID MK_LPAREN relop_expr_list MK_RPAREN{
			$$=check_function($2,$4);
			if(($$->type!=ERROR_)&&($$->type!=INT_)&&($$->type!=FLOAT_)){
				printf("error %d: operator ! applied to non integer expression\n",linenumber);
				$$->type=ERROR_;
			}
		}


		| var_ref {$$=$1;}
		| OP_MINUS var_ref{
			if(($2->type!=INT_)&&($2->type!=FLOAT_)){
				printf("error %d: operator Unary Minus applied to non Basic type %s\n",linenumber,$2->name);
				$2->type=ERROR_;
			}
			$$=$2;
		}
		| OP_NOT var_ref{
			if($2->type!=INT_){
				if($2->type!=FLOAT_){
					printf("error %d: operator ! applied to non scalar type%s\n",linenumber,$2->name);
					$2->type=ERROR_;
				}else{
					printf("warning %d: operator ! applied to a float constant\n",linenumber);
					$2->type=INT_;
				}
			}
			$$=$2;
		}

		;
var_ref		: ID{
			symtab * STP;
			$$=Allocate(VAR_REF);
			if(((STP=lookup($1))==NULL)||(STP->scope>scope)){
				printf("error %d: undeclared variable %s\n",linenumber,$1);
				$$->type=ERROR_;
			}else if(STP->type==FUNC_||STP->type==TYPEDEF_){
				printf("error %d: referenced symbol %s is not a variable \n",linenumber,$1);
				$$->type=ERROR_;
			}
			else if(STP->type==ERROR_)
				$$->type=ERROR_;
			else{
				$$->type=STP->type;
				$$->name=$1;

				if($$->type==STR_VAR_){
					$$->var_ref_u.type_name=STP->symtab_u.type_name;
				}

/*we need do to this beacuse not all arrays are found inside the symbol table*/
/*not sure if needed*/

				if($$->type==ARR_){
					Type_arr *PTA;
					PTA=Allocate(TYPE_ARR);
					PTA->dim=STP->symtab_u.st_arr->dim;
					PTA->arrtype=STP->symtab_u.st_arr->arrtype;
					PTA->type_name=STP->symtab_u.st_arr->type_name;
					$$->var_ref_u.arr_info=PTA;
				$$->place = get_reg($$);
				}
			}
		}
		|var_ref dim{
			if($1->type==ERROR_)
				$$=$1;
			else{
				if($1->type!=ARR_){
					symtab *STP;
					STP=lookup($1->name);

					if((STP)&&(STP->type==ARR_))
						printf("error %d: array %s has only %d dimension(s)\n",linenumber,$1->name,STP->symtab_u.st_arr->dim);
					else
						printf("error %d: variable not an array %s\n", linenumber,$1->name);
					$1->type=ERROR_;
					GLOBAL_ERROR=1;
				}
				else if(($2->type!=ERROR_)&&($2->type!=INT_)){
					printf("error %d: dimension is not an integer in array variable %s\n",linenumber,$1->name);
					$1->type=ERROR_;
					GLOBAL_ERROR=1;
				}
				else{
					int i;
					/*
					fprintf(stderr, "var_ref tmp_intval = %d\n", $2->tmp_val_u.tmp_intval);
					fprintf(stderr, "setting arr[%d] to %d\n", $1->var_ref_u.arr_info->dim, $2->tmp_val_u.tmp_intval);
					*/
					$1->var_ref_u.arr_info->dim_limit[$1->var_ref_u.arr_info->dim - 1] = $2->tmp_val_u.tmp_intval;
					i=--$1->var_ref_u.arr_info->dim;

					if(i==0){

						/*we have reached the variable in the array*/

						$1->type=$1->var_ref_u.arr_info->arrtype;
						if($1->type==STR_){
							$1->type=STR_VAR_;
							$1->var_ref_u.type_name=$1->var_ref_u.arr_info->type_name;
						}
					}
					else
						;
			}
			$$=$1;
			}
		}

		| var_ref struct_tail{
			/*var_ref must be a structure. check if it has member struct tail*/

			struct_semantic * STRSEMP;
			if($1->type==ERROR_)
				$$=$1;
			else if($1->type!=STR_VAR_){
				printf("error %d: variable %s is not a structure\n",linenumber,$1->name);
				$1->type=ERROR_;
				$$=$1;
			}
			else if((STRSEMP=search($1->var_ref_u.type_name,$2))==NULL){
				printf("error %d: struct variable %s has no member named %s\n",linenumber,$1->name,$2);
				$1->type=ERROR_;
				$$=$1;
			}
			else{
				$1->type=STRSEMP->type;
				$1->name=Add_Str($1->name,$2);
				if($1->type==STR_VAR_)
					$1->var_ref_u.type_name=STRSEMP->struct_semantic_u.str_info.struct_type_name;
				if($1->type==ARR_){
					$1->var_ref_u.arr_info=Allocate(TYPE_ARR);
					$1->var_ref_u.arr_info->dim=STRSEMP->struct_semantic_u.arr_sem->arr_info->dim;
					$1->var_ref_u.arr_info->arrtype=STRSEMP->struct_semantic_u.arr_sem->arr_info->arrtype;
					$1->var_ref_u.arr_info->type_name=STRSEMP->struct_semantic_u.arr_sem->arr_info->type_name;
				}
				$$=$1;
			}
		}
		;


dim		: MK_LB expr MK_RB{$$=$2;}
		;

struct_tail	: MK_DOT ID {$$=Allocate(STRING);$$=$2;}
		;

%%

void
VerifyMainCall ()
{
    struct symtab *temp;

    temp = lookup ("main");
    if (temp != NULL)
    {
        if ((temp->scope == 0) && (temp->type == FUNC_)
            && (temp->symtab_u.st_func->params == 0))
            return;
    }
    printf ("Warning: no valid Main function\n");
}

/*
vim: noexpandtab shiftwidth=8 tabstop=8
*/
