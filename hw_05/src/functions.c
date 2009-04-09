#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <header.h>
#include <y.tab.h>

extern int scope;
extern int STRUCT_DEC;
extern int linenumber;
extern int GLOBAL_ERROR;

extern int linenumber;

char *printarray[] =
    { "int", "float", "array", "struct", "function", "typedef", "void",
    "error type"
};

char *
printtype (int x)
{
    switch (x)
    {
    case INT_:
        return printarray[0];
    case FLOAT_:
        return printarray[1];
    case ARR_:
        return printarray[2];
    case STR_:
    case STR_VAR_:
        return printarray[3];
    case FUNC_:
        return printarray[4];
    case TYPEDEF_:
        return printarray[5];
    case VOID_:
        return printarray[6];
    default:
        return printarray[7];
    }
}

var_ref *
relop_extm (var_ref * a, int b, var_ref * c)
{
    if (a->type == ERROR_ || c->type == ERROR_)
        a->type = ERROR_;
    else if (a->type != INT_ || c->type != INT_)
    {
        if ((a->type == INT_ || a->type == FLOAT_)
            && (c->type == INT_ || c->type == FLOAT_))
        {
            printf ("warning %d: operator %s applied to non integer type\n",
                    linenumber, b == OP_OR ? "||" : "&&");
            a->type = INT_;
        }
        else
        {
            printf ("error %d: operator %s applied to non basic type\n",
                    linenumber, b == OP_OR ? "||" : "&&");
            a->type = ERROR_;
        }
    }
    else
        a->type = INT_;
    return a;
}

var_ref *
assign_ex (char *a, var_ref * b)
{
    symtab *PST;

    if ((PST = lookup (a)) == NULL)
    {
        printf ("error %d: undeclared variable %s\n", linenumber, a);
        b->type = ERROR_;
        GLOBAL_ERROR = 1;
        return b;
    }
    if ((PST->type == ERROR_) || (b->type == ERROR_))
    {
        b->type = ERROR_;
        return b;
    }
    if (PST->type != b->type)
    {
        if ((PST->type == INT_ || PST->type == FLOAT_)
            && (b->type == INT_ || b->type == FLOAT_))
            b->type = ((PST->type == FLOAT_)
                       || (b->type == FLOAT_)) ? FLOAT_ : INT_;
        else
        {
            printf ("error %d: incompatible assignment, %s %sto %s %s\n",
                    linenumber, printtype (b->type), (b->name) ? b->name : "",
                    printtype (PST->type), PST->lexeme);
            b->type = ERROR_;
        }
    }
    else
        switch (PST->type)
        {
        case STR_VAR_:
            if (PST->symtab_u.type_name != b->var_ref_u.type_name)
            {
                printf
                    ("error %d: incompatible assignment, struct %s to struct %s of different types\n",
                     linenumber, (b->name) ? (b->name) : "", a);
                b->type = ERROR_;
            }
            break;
        case ARR_:
            printf
                ("error %d: incompatible assignment array %s to array %s\n",
                 linenumber, (b->name) ? (b->name) : "", a);
            b->type = ERROR_;
            break;
        case INT_:
        case FLOAT_:
            break;
        default:
            printf ("CHECKER ERROR: unknown types, line %d\n", linenumber);
            break;
        }
    return b;
}

TYPE
stmt_assign_ex (var_ref * a, var_ref * b)
{
    if ((a->type == ERROR_) || (b->type == ERROR_))
        return ERROR_;
    if (a->type != b->type)
    {
        if ((a->type == INT_ || a->type == FLOAT_)
            && (b->type == INT_ || b->type == FLOAT_))
            return ZERO_;
        else
        {
            printf ("error %d: incompatible assignment, %s %s to %s %s\n",
                    linenumber, printtype (b->type),
                    (b->name) ? (b->name) : "", printtype (a->type),
                    (a->name) ? a->name : "");
            return ERROR_;
        }
    }
    else
        switch (a->type)
        {
        case STR_VAR_:
            if (a->var_ref_u.type_name != b->var_ref_u.type_name)
            {
                printf
                    ("error %d: incomaptible assignment, struct %s to struct %s of different types\n",
                     linenumber, (b->name) ? (b->name) : "", a->name);
                return ERROR_;
            }
            break;
        case ARR_:
            printf
                ("error %d: incompatible assignment array %s to array %s\n",
                 linenumber, (b->name) ? (b->name) : "", a->name);
            return ERROR_;
            break;
        case INT_:
        case FLOAT_:
            return ZERO_;
            break;

        default:
            printf ("CHECKER ERROR: unknown types, line %d\n", linenumber);
            return ERROR_;
            break;
        }
    return ZERO_;
}




TYPE
check_relop_ex_lst (TypeList * a)
{
    if (a == NULL)
        return ZERO_;
    do
    {
        if (a->P_var_r->type == ERROR_)
            return ERROR_;
    }
    while ((a = a->next));
    return ZERO_;
}


id_list *
Create_Id_List (char *x)
{
    init_id *PII;
    id_list *RET;
    PII = Allocate (INIT_ID);
    PII->init_id_u.name = x;
    PII->type = ZERO_;
    RET = Allocate (ID_LIST);
    RET->P_ini_i = PII;
    RET->next = NULL;
    return RET;
}

id_list *
Merge_Id_List (id_list * y, char *x)
{
    init_id *PII;
    id_list *RET;
    PII = Allocate (INIT_ID);
    PII->init_id_u.name = x;
    PII->type = ZERO_;
    RET = Allocate (ID_LIST);
    RET->P_ini_i = PII;
    RET->next = y;
    return RET;
}

id_list *
Id_List_Array (id_list * a, char *b, ArrayInfo * c)
{
    init_id *PII;
    id_list *RET;
    int j;
    PII = Allocate (INIT_ID);
    PII->type = ARR_;
    PII->init_id_u.P_arr_s = Allocate (ARRAY_SEM);
    PII->init_id_u.P_arr_s->arr_info = Allocate (TYPE_ARR);
    PII->init_id_u.P_arr_s->name = b;
    PII->init_id_u.P_arr_s->arr_info->dim = c->dim;
    for (j = 0; j < 10; j++)
    {
        PII->init_id_u.P_arr_s->arr_info->dim_limit[j] = c->dim_limit[j];
    }
    RET = Allocate (ID_LIST);
    RET->P_ini_i = PII;
    if (a == NULL)
        RET->next = NULL;
    else
        RET->next = a;
    return RET;
}

def_list *
def_list_P (def_list * a, var_decl * b)
{
    def_list *PDL;
    PDL = Allocate (DEF_LIST);
    PDL->P_var_s = b;
    if (a == NULL)
        PDL->next = NULL;
    else
        PDL->next = a;
    return PDL;
}


struct token
{
    char *name;
    struct token *left;
    struct token *right;
};

struct token *root;

void
init (struct token *ptr)
{
    ptr->name = NULL;
    ptr->left = NULL;
    ptr->right = NULL;
    return;
}

int addtreeflag = 0;
void
addtree (struct token **TT, char *S)
{
    struct token *T;
    int i;
    T = *TT;
    if (T == NULL)
    {
        T = (struct token *) malloc (sizeof (struct token));
        init (T);
        T->name = S;
        *TT = T;
        return;
    }
    i = strcmp (T->name, S);
    if (i == 0)
    {
        printf ("error %d: duplicate tokens %s in structure definition\n",
                linenumber, S);
        addtreeflag = 1;
    }
    else if (i < 0)
        addtree (&(T->right), S);
    else
        addtree (&(T->left), S);
}

int
check_duplicate (def_list * a)
{
    var_decl *PVD;
    addtreeflag = 0;
    root = NULL;
    do
    {
        id_list *PIL;
        PVD = a->P_var_s;
        PIL = PVD->P_id_l;
        do
        {
            init_id *PII = PIL->P_ini_i;
            switch (PII->type)
            {
            case ARR_:
                addtree (&root, PII->init_id_u.P_arr_s->name);
                break;
            default:
                addtree (&root, PII->init_id_u.name);
                break;
            }
        }
        while ((PIL = PIL->next));
    }
    while ((a = a->next));

    return addtreeflag;
}



/*return 0 on error else return 1*/
/* 0 means no entry added to symbol table*/
int
struct_type_P (char *a, def_list * b)
{
    struct_semantic *ret = NULL;
    struct_semantic *PSS;

    if (b == NULL)
    {
        printf ("error %d: structure declaration with no members\n",
                linenumber);
        GLOBAL_ERROR = 1;
        return 0;
    }
    if (check_duplicate (b))
    {
        GLOBAL_ERROR = 1;
        return 0;
    }

    /*there are no duplicates in the structure */
    /*we enter all the elements in the def_list even of they have errors */

    do
    {
        var_decl *PVD = b->P_var_s;
        TYPE type = PVD->type;
        id_list *PIL = PVD->P_id_l;
        do
        {
            init_id *PII = PIL->P_ini_i;
            PSS = Allocate (STRUCT_SEM);
            switch (type)
            {
            case INT_:
            case FLOAT_:
                switch (PII->type)
                {
                case ZERO_:
                case ERROR_:
                    PSS->struct_semantic_u.var_name = PII->init_id_u.name;
                    PSS->type = type;
                    break;
                case ARR_:
                    PSS->struct_semantic_u.arr_sem = PII->init_id_u.P_arr_s;
                    PSS->type = ARR_;
                    PII->init_id_u.P_arr_s->arr_info->arrtype = type;
                    break;
                default:
                    break;
                }
                break;
            case STR_VAR_:
            case STR_:
                switch (PII->type)
                {
                case ZERO_:
                case ERROR_:
                    PSS->type = type;
                    PSS->struct_semantic_u.str_info.str_var_name =
                        PII->init_id_u.name;
                    PSS->struct_semantic_u.str_info.struct_type_name =
                        PVD->type_name;
                    break;
                case ARR_:
                    PSS->type = ARR_;
                    PSS->struct_semantic_u.arr_sem = PII->init_id_u.P_arr_s;
                    PII->init_id_u.P_arr_s->arr_info->arrtype = STR_;
                    PII->init_id_u.P_arr_s->arr_info->type_name =
                        PVD->type_name;
                    break;
                default:
                    break;
                }
                break;
            case ERROR_:
                GLOBAL_ERROR = 1;
                continue;
            default:
                GLOBAL_ERROR = 1;
                continue;
            }
            if (!ret)
            {
                PSS->next = NULL;
                ret = PSS;
            }
            else
            {
                PSS->next = ret;
                ret = PSS;
            }

        }
        while ((PIL = PIL->next));
    }
    while ((b = b->next));
    insert (a, STR_, ret, 0);
    return 1;
}


char *buffer;
int tempnumber = 0;
char *
get_temp_name ()
{

    buffer = (char *) malloc (10);
    sprintf (buffer, "&&temp%d", tempnumber);
    tempnumber++;
    return buffer;

}

char *
ArrayNewName (char *a)
{
    int i;
    char *c;
    i = strlen (a);
    c = (char *) malloc (i + 2 + 1);
    strcpy (c, a);
    strcat (c, "[]");
    return c;
}

char *
new_name (char *a)
{
    int i;
    char *c;
    i = strlen (a);
    c = (char *) malloc (i + 2);
    *c = '&';
    *(c + 1) = '\0';
    strcat (c, a);
    return c;
}

int
if_user_name (char *x)
{
    if (*x == '&' && *(x + 1) == '&')
        return 0;
    return 1;
}

TYPE
type_decl_enter_ST1 (int a, id_list * b)
{
    /*enter the id_list into the ST */
    char *name;
    symtab *entry;
    TYPE ret_type = ZERO_;
    do
    {
        name =
            (b->P_ini_i->type ==
             ARR_) ? b->P_ini_i->init_id_u.P_arr_s->name : b->
            P_ini_i->init_id_u.name;
        if ((entry = lookup (name)))
            if ((entry) && (entry->scope >= scope))
            {
                printf
                    ("error %d: variable or type redeclared, %s, declared earlier at line %d\n",
                     linenumber, name, entry->line);
                ret_type = ERROR_;
            }
        if (b->P_ini_i->type == ARR_)
        {
            b->P_ini_i->init_id_u.P_arr_s->arr_info->arrtype = a;
            insert (name, TYPEDEF_, b->P_ini_i->init_id_u.P_arr_s->arr_info,
                    TYPEDEF_ARR);
        }
        else if (a == INT_)
            insert (b->P_ini_i->init_id_u.name, TYPEDEF_, NULL, TYPEDEF_INT);
        else
            insert (b->P_ini_i->init_id_u.name, TYPEDEF_, NULL, TYPEDEF_FLT);
    }
    while ((b = b->next));
    return ret_type;
}

TYPE
type_decl_enter_ST2 (char *a, id_list * b)
{
    char *name;
    symtab *entry;
    TYPE ret_type = ZERO_;
    entry = lookup (a);
    if (entry == NULL)
    {
        printf ("error %d:  type undeclared, %s\n", linenumber, name);
        ret_type = ERROR_;
    }
    else
        do
        {
            name =
                (b->P_ini_i->type ==
                 ARR_) ? b->P_ini_i->init_id_u.P_arr_s->name : b->
                P_ini_i->init_id_u.name;
            if (b->P_ini_i->type == ARR_)
            {
                b->P_ini_i->init_id_u.P_arr_s->arr_info->arrtype = STR_;
                b->P_ini_i->init_id_u.P_arr_s->arr_info->type_name = a;
                insert (name, TYPEDEF_,
                        b->P_ini_i->init_id_u.P_arr_s->arr_info, TYPEDEF_ARR);
            }
            else
                insert (name, TYPEDEF_, a, TYPEDEF_STR);
        }
        while ((b = b->next));
    return ret_type;
}


void
chk_insert (char *a, TYPE b, void *c, IS_TYPE_DEF d)
{
    symtab *PST;
    PST = lookup (a);
    if ((PST) && (PST->scope >= scope))
    {
        printf
            ("error %d:redeclaration of variable %s declared previously at line %d\n",
             linenumber, a, PST->line);
        GLOBAL_ERROR = 1;
    }
    else
        insert (a, b, c, d);
    return;
}



TYPE
decl_enter_ST (var_decl * a)
{
    init_id *PII;
    id_list *PIL;
    TYPE ret = ZERO_;
    PIL = a->P_id_l;

    if (a == NULL)
        return ERROR_;

    if (a->type == ERROR_)
        return ERROR_;



    PIL = a->P_id_l;
    if (PIL == NULL)
        return ERROR_;

    do
    {
        PII = PIL->P_ini_i;
        switch (a->type)
        {
        case INT_:
            if (PII->type == ARR_)
            {
                PII->init_id_u.P_arr_s->arr_info->arrtype = INT_;
                chk_insert (PII->init_id_u.P_arr_s->name, ARR_,
                            PII->init_id_u.P_arr_s->arr_info, 0);
            }
            else
                chk_insert (PII->init_id_u.name, INT_, NULL, 0);
            break;
        case FLOAT_:
            if (PII->type == ARR_)
            {
                PII->init_id_u.P_arr_s->arr_info->arrtype = FLOAT_;
                chk_insert (PII->init_id_u.P_arr_s->name, ARR_,
                            PII->init_id_u.P_arr_s->arr_info, 0);
            }
            else
                chk_insert (PII->init_id_u.name, FLOAT_, NULL, 0);
            break;
        case STR_VAR_:
        case STR_:
            if (PII->type == ARR_)
            {
                PII->init_id_u.P_arr_s->arr_info->arrtype = STR_;
                PII->init_id_u.P_arr_s->arr_info->type_name = a->type_name;
                chk_insert (PII->init_id_u.P_arr_s->name, ARR_,
                            PII->init_id_u.P_arr_s->arr_info, 0);
            }
            else
                chk_insert (PII->init_id_u.name, STR_VAR_, a->type_name, 0);
            break;
        case ERROR_:
            ret = ERROR_;
            break;
        default:
            ret = ERROR_;
            break;
        }
    }
    while ((PIL = PIL->next));

    return ret;
}

char *
Add_Str (char *a, char *b)
{
    int i;
    char *c;
    i = strlen (a) + strlen (b);
    c = (char *) malloc (i + 1 + 4);
    strcpy (c, b);
    strcat (c, " in ");
    strcat (c, a);
    return c;
}

struct_semantic *
search (char *a, char *b)
{
    struct_semantic *PSS1;
    symtab *PST, *PST1;
    char c[32];
    int i = 0;

    while (((c[i] = *b++) != ' ') && (c[i] != '\0'))
        i++;
    c[i] = '\0';

    if ((PST = lookup (a)) == NULL)
    {
        printf ("error %d: unknown struct type variable %s\n", linenumber, a);
        GLOBAL_ERROR = 1;
        return NULL;
        /*exit(0); */
    }

    if (PST->scope > scope)
    {
        printf ("error %d: undeclared struct type variable %s\n", linenumber,
                a);
        GLOBAL_ERROR = 1;
        return NULL;
    }
    PSS1 = PST->symtab_u.st_struct;

    while (PSS1)
    {
        switch (PSS1->type)
        {
        case ARR_:
            if (!strcmp (c, PSS1->struct_semantic_u.arr_sem->name))
                return PSS1;
            break;
        case STR_:
            if (!strcmp (c, PSS1->struct_semantic_u.str_info.str_var_name))
            {
                if (((PST1 =
                      lookup (PSS1->struct_semantic_u.
                              str_info.struct_type_name)) == NULL)
                    || (PST1->scope > scope))
                    return NULL;
                else
                    return PSS1;
            }
            break;
        default:
            if (!strcmp (c, PSS1->struct_semantic_u.var_name))
                return PSS1;
            break;
        }
        PSS1 = PSS1->next;
    }
    return PSS1;
}


TYPE
param_P (param * a, char *b)
{
    Type_arr *PTA;
    symtab *PST;
    if (a == NULL)
        return ZERO_;


    if (((PST = lookup (b)) != NULL) && (PST->scope >= scope))
    {
        printf ("error %d: redeclaration of variable %s in parameter list\n",
                linenumber, b);
        return ERROR_;
    }

    else
    {
        switch (a->type)
        {
        case INT_:
            insert (b, INT_, NULL, 0);
            break;
        case FLOAT_:
            insert (b, FLOAT_, NULL, 0);
            break;
        case ARR_:
            PTA = Allocate (TYPE_ARR);
            PTA->dim = a->dim;
            PTA->arrtype = a->arrtype;
            insert (b, ARR_, PTA, 0);
            break;
        default:
            break;
        }
    }
    return ZERO_;
}

param_list *
MakeParamList (param_list * a, param * b)
{
    param_list *c, *d = a;


    if (a == NULL)
    {
        a = Allocate (STRINGLIST);
        a->PPAR = b;
        a->next = NULL;
        return a;
    }

    else
    {
        do
        {
            c = a;
        }
        while ((a = a->next));

        c->next = Allocate (PARAM_LIST);
        c->next->PPAR = b;
        c->next->next = NULL;
        if (b == NULL)
            GLOBAL_ERROR = 1;
    }
    return d;
}


TYPE
func_enter_ST (TYPE a, char *b, param_list * c)
{
    ST_func *PSF;
    symtab *PST;
    TYPE ret = ZERO_;
    int i = 0;
    if ((PST = lookup (b)) != NULL)
    {
        printf
            ("error %d: redeclaration of symbol %s  as function, declared previously as %s on line %d\n",
             linenumber, b, printtype (PST->type), PST->line);
        return ERROR_;
    }

    PSF = Allocate (ST_FUNC);
    PSF->PL = c;
    while (c)
    {
        i++;
        if (c->PPAR == NULL)
            ret = ERROR_;
        c = c->next;
    }
    PSF->params = i;
    switch (a)
    {
    case INT_:
    case FLOAT_:
    case VOID_:
        PSF->ret_type = a;
        break;
    case STR_:
        PSF->ret_type = ERROR_;
        ret = ERROR_;
        break;
    default:
        PSF->ret_type = ERROR_;
        ret = ERROR_;
    }

    insert (b, FUNC_, PSF, 0);
    return ret;
}



var_ref *
check_function (char *a, TypeList * b)
{
    symtab *PST;
    var_ref *PVR;
    int i = 0;
    TypeList *c = b;
    while (c)
    {
        i++;
        c = c->next;
    }

    PVR = Allocate (VAR_REF);

    if ((PST = lookup (a)) == NULL)
    {
        printf ("error %d: undefined function %s\n", linenumber, a);
        PVR->type = ERROR_;
        GLOBAL_ERROR = 1;
    }
    else if (PST->type != FUNC_)
    {
        printf ("error %d: %s is not a function\n", linenumber, a);
        PVR->type = ERROR_;
        GLOBAL_ERROR = 1;
    }
    else if (i > PST->symtab_u.st_func->params)
    {
        printf ("error %d: too many arguments to function %s\n", linenumber,
                a);
        PVR->type = ERROR_;
        GLOBAL_ERROR = 1;
    }
    else if (i < PST->symtab_u.st_func->params)
    {
        printf ("error %d: too few arguments to function %s\n", linenumber,
                a);
        PVR->type = ERROR_;
        GLOBAL_ERROR = 1;
    }
    else
    {
        var_ref *PVR1;
        ST_func *x;
        param_list *y;
        x = PST->symtab_u.st_func;
        y = x->PL;
        i = 0;
        PVR->type = PST->symtab_u.st_func->ret_type;
        while (y)
        {
            PVR1 = b->P_var_r;
            if ((y->PPAR == NULL) || (PVR1->type == ERROR_))
            {
                y = y->next;
                b = b->next;
                PVR->type = ERROR_;
                i++;
                continue;
            }


            switch (y->PPAR->type)
            {
            case INT_:
            case FLOAT_:
                if ((PVR1->type != INT_) && (PVR1->type != FLOAT_))
                {
                    printf
                        ("error %d: function arg %d, expects scalar, passed %s of type %s\n",
                         linenumber, i, (PVR1->name) ? (PVR1->name) : "",
                         printtype (PVR1->type));
                    PVR->type = ERROR_;
                }
                break;
            case ARR_:
                if ((PVR1->type == INT_) || (PVR1->type == FLOAT_))
                {
                    printf
                        ("error %d: function arg %d, scalar %s passed, expects a %dD array\n",
                         linenumber, i, (PVR1->name) ? (PVR1->name) : "",
                         PST->symtab_u.st_arr->dim);
                    PVR->type = ERROR_;
                }
                else if (PVR1->type == ARR_)
                {
                    if ((y->PPAR->arrtype !=
                         PVR1->var_ref_u.arr_info->arrtype)
                        || (y->PPAR->dim != PVR1->var_ref_u.arr_info->dim))
                        printf
                            ("warning %d: passing arg %d of %s from incompatible pointer type\n",
                             linenumber, i, a);
                }
                else
                {
                    printf
                        ("error %d: function arg %d, expects array, passed a struct %s\n",
                         linenumber, i, (PVR1->name) ? (PVR1->name) : "");
                    PVR->type = ERROR_;
                }
                break;
            default:
                break;
            }
            i++;
            y = y->next;
            b = b->next;
        }
    }

    return PVR;
}

void
put_read_ST ()
{
    ST_func *PSF;

    PSF = Allocate (ST_FUNC);
    PSF->ret_type = INT_;
    PSF->params = 0;
    PSF->PL = NULL;

    insert ("read", FUNC_, PSF, 0);

    PSF = Allocate (ST_FUNC);
    PSF->params = 0;
    PSF->ret_type = FLOAT_;
    PSF->PL = NULL;

    insert ("fread", FUNC_, PSF, 0);

    return;
}

TYPE
check_return (int flag, TYPE type)
{
    if (flag)
        return ZERO_;
    else
    {
        printf ("error %d: missing return statement\n", linenumber);
        return ERROR_;
    }
}

/*we do not allocate the string for the lexeme here*/
void *
Allocate (ALL_TYPE type)
{
    switch (type)
    {
    case STRING:
        return (void *) malloc (sizeof (String));
    case INTNUMBER:
        return (void *) malloc (sizeof (int));
    case TYPE_STRING:
        return (void *) malloc (sizeof (Type_String));
    case TYPELIST:
        return (void *) malloc (sizeof (TypeList));
    case STRINGLIST:
        return (void *) malloc (sizeof (Stringlist));
    case ARRAY_SEM:
        return (void *) malloc (sizeof (array_semantic));
    case STRUCT_SEM:
        return (void *) malloc (sizeof (struct_semantic));
    case ID_LIST:
        return (void *) malloc (sizeof (id_list));
    case VAR_DECL:
        return (void *) malloc (sizeof (var_decl));
    case INIT_ID:
        return (void *) malloc (sizeof (init_id));
    case DEF_LIST:
        return (void *) malloc (sizeof (def_list));
    case PARAM:
        return (void *) malloc (sizeof (param));
    case ST_ARR:
        return (void *) malloc (sizeof (ST_arr));
    case ST_FUNC:
        return (void *) malloc (sizeof (ST_func));
    case ST_STRUCT:
        return (void *) malloc (sizeof (ST_struct));
    case SYMTAB:
        return (void *) malloc (sizeof (symtab));
    case VAR_REF:
        return (void *) malloc (sizeof (var_ref));
    case TYPE_ARR:
        return (void *) malloc (sizeof (Type_arr));
    case PARAM_LIST:
        return (void *) malloc (sizeof (param_list));
    case CONST_REC:
        return (void *) malloc (sizeof (CON_Type));
    default:
        return NULL;
    }
}

/*
vim: expandtab shiftwidth=4 tabstop=4 smarttab
*/
