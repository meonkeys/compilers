#ifndef HEADER_H
#define HEADER_H

/* Constant Types */
typedef enum
{ INTEGERC, FLOATC, STRINGC } C_type;

typedef enum TYPE_e
{ ZERO_, INT_, FLOAT_, ARR_, STR_, VOID_, ERROR_, CONST_, FUNC_, POINTER_,
    STR_VAR_, STRING_, TYPEDEF_
} TYPE;

typedef enum
{ INTNUMBER, STRING, TYPE_STRING, TYPELIST, STRINGLIST, ARRAY_SEM, STRUCT_SEM,
    ID_LIST, VAR_DECL, INIT_ID, DEF_LIST, VAR_REF, PARAM, PARAM_LIST,
    ST_ARR, ST_FUNC, ST_STRUCT, ST_BASIC, SYMTAB, TYPE_ARR,
    CONST_REC
} ALL_TYPE;

typedef struct CON_Type_s
{
    C_type const_type;
    union
    {
        int intval;
        double fval;
        char *sc;
    }
    const_u;
} CON_Type;

typedef struct MyArrInfo
{
    int dim;
    int dim_limit[10];
} ArrayInfo;

typedef struct
{
    char *name;
} String;

typedef struct
{
    TYPE type;
    char *name;
} Type_String;


struct Stringlist
{
    char *name;
    struct Stringlist *next;
};
typedef struct Stringlist Stringlist;

typedef struct
{
    int dim;
    int dim_limit[10];
    TYPE arrtype;
    char *type_name;            /*in case of array of structs */
    /*or in case of array of array through typedef */
} Type_arr;

typedef struct
{
    Type_arr *arr_info;
    char *name;                 /*name of array in symbol table, filled in init_id or id_list */
} array_semantic;

struct struct_semantic
{
    TYPE type;
    union
    {
        char *var_name;         /*name of the variable of type type. */
        array_semantic *arr_sem;        /*info if the element is an array */
        struct
        {
            char *str_var_name; /*name of variable in case of nested struct */
            char *struct_type_name;     /*if TYPE is STR_R, this is the name of
                                           type of struct in symbol table
                                           this name is a temporary name since structs
                                           do not have tags, but if they do have tags,
                                           it is handled in the same way */
        } str_info;
    } struct_semantic_u;
    struct struct_semantic *next;
};
typedef struct struct_semantic struct_semantic;

typedef struct var_ref_s
{
    TYPE type;
    char *name;
    int place;
    int label_num;
    union
    {
        char *type_name;
        Type_arr *arr_info;
    } var_ref_u;
    union
    {
        int tmp_intval;         /* Temporarily hold value until we can emit it in assembly */
        float tmp_fval;         /* or store it in the symbol table. */
        char* tmp_str;
    } tmp_val_u;
} var_ref;

struct TypeList
{
    var_ref *P_var_r;
    struct TypeList *next;
};
typedef struct TypeList TypeList;

typedef struct init_id_s
{
    TYPE type;                  /*the type tells only if an array or not */
    int assignment_during_initialization;
    int offset;
    /*we cannot have struct types here. */
    union
    {
        char *name;
        array_semantic *P_arr_s;
    } init_id_u;
    union
    {
        int intval;
        float fval;
    } val_u;
} init_id;

/*explanations above in id_list structure*/
struct id_list
{
    init_id *P_ini_i;
    struct id_list *next;
};
typedef struct id_list id_list;

typedef struct var_decl_s
{
    TYPE type;
    char *type_name;            /*if type is STR_, this is the name of struct type in ST */
    /*if a typedef of struct, name of ST entry
       if a typedef of array, name of ST entry */

    id_list *P_id_l;
    int linnum;                 /*line number of declaration for
                                   error messages; */
} var_decl;

struct def_list
{
    var_decl *P_var_s;
    struct def_list *next;
};
typedef struct def_list def_list;


typedef struct param_s
{
    TYPE type;                  /*type, could be basic type, array or error */
    char *name;
    TYPE arrtype;               /*if type is ARR_, this the type of array */
    int dim;                    /*if array number of dimensions */
    int offset;                 /* needed for asm gen */
} param;

struct param_list
{
    param *PPAR;
    struct param_list *next;
};
typedef struct param_list param_list;


/*Symbol table entries for basic types, arrays, functions, and structs*/
typedef Type_arr ST_arr;

typedef struct
{
    TYPE ret_type;
    int params;
    param_list *PL;
} ST_func;

typedef struct_semantic ST_struct;

typedef enum
{ TYPEDEF_INT, TYPEDEF_FLT, TYPEDEF_ARR, TYPEDEF_STR } IS_TYPE_DEF;

struct symtab
{
    char *lexeme;
    TYPE type;
    IS_TYPE_DEF type_when_def;
    union
    {
        ST_arr *st_arr;         /*for arrays not nested inside a struct */
        ST_func *st_func;       /*for function declaration */
        ST_struct *st_struct;   /*for structure declaration */
        char *type_name;        /*for structure variables */
        /*also name of struct type in case of
           typedef structs */
    } symtab_u;
    struct symtab *front;
    struct symtab *back;
    int scope;
    int line;
    int place;
    int offset;
    union
    {
        int intval;
        float fval;
    } val_u;
};


typedef struct symtab symtab;
symtab *lookup (char *name);
symtab *insert (char *name, TYPE type, void *P, IS_TYPE_DEF TypeDef);
int delete_scope (int scp);
void *Allocate (ALL_TYPE);
char *printtype (int x);
char *ArrayNewName (char *a);
var_ref *relop_extm (var_ref * a, int b, var_ref * c);
var_ref *assign_ex (char *a, var_ref * b);
TYPE check_relop_ex_lst (TypeList * a);
id_list *Create_Id_List (char *x);
id_list *Merge_Id_List (id_list * y, char *x);
id_list *Id_List_Array (id_list * a, char *b, ArrayInfo * c);
def_list *def_list_P (def_list * a, var_decl * b);
int struct_type_P (char *a, def_list * b);
char *get_temp_name ();
char *new_name (char *a);
int if_user_name (char *x);
TYPE type_decl_enter_ST1 (int a, id_list * b);
TYPE type_decl_enter_ST2 (char *a, id_list * b);
TYPE decl_enter_ST (var_decl * a);
TYPE stmt_assign_ex (var_ref * a, var_ref * b);
char *Add_Str (char *a, char *b);
struct_semantic *search (char *a, char *b);
var_ref *check_function (char *a, TypeList * b);
TYPE func_enter_ST (TYPE a, char *b, param_list * c);
param_list *MakeParamList (param_list * a, param * b);
TYPE param_P (param * a, char *b);
void put_read_ST ();
TYPE check_return (int flag, TYPE type);
void asm_out (char const *fmt, ...);
void asm_emit_global_decls_start (void);
void asm_emit_global_decl_list (var_decl * v);
void asm_emit_scoped_decl_list (var_decl * v);

int asm_emit_expr (var_ref * a, var_ref * b, int opval);
int asm_emit_term (var_ref * a, var_ref * b, int opval);

void asm_emit_write (TypeList * idl);
void asm_emit_read ();
void asm_emit_fread ();

int set_var_decl_list_offsets (var_decl * v, int offset);
void set_param_list_offsets (param_list * pl);
int set_var_decl_offset (var_decl * v);

id_list *reverse_id_list (id_list * p);

void gen_prologue (const char *name);
void gen_epilogue (const char *name);

/* start/continue of while, if-else, for */
void gen_control_start (int test_label_num);
void gen_control_test (var_ref * a, int exit_label_num);
void gen_control_iterate (int test_label_num);
void gen_control_exit (int exit_label_num);

int get_reg (var_ref * vr);
int get_result_reg ();

void frame_data_out(char const* fmt, ...);

void free_reg (int r);
void ns_reg (int r);
void save_reg (int r);
#endif

/*
vim: expandtab shiftwidth=4 tabstop=4 smarttab
*/
