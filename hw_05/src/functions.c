#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <header.h>
#include <y.tab.h>

extern int scope;
extern int STRUCT_DEC;
extern int linenumber;
extern int GLOBAL_ERROR;

extern int linenumber;

extern int offset;
extern int param_offset;

extern char *frame_data;

extern FILE *asm_out_fp;

int cur_const_val;

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

/* Force variable type to match given type. */
void
autocast (TYPE type, init_id * PII)
{
#if 0
    fprintf (stderr, "autocast: %s, %s intval=%d fval=%f\n", printtype (type),
             PII->init_id_u.name, PII->val_u.intval, PII->val_u.fval);
#endif
    switch (type)
    {
    case INT_:
        if (FLOAT_ == PII->type)
            PII->val_u.intval = PII->val_u.fval;
        break;
    case FLOAT_:
        if (INT_ == PII->type)
            PII->val_u.fval = PII->val_u.intval;
        break;
    default:
        fprintf (stderr, "Cannot autocast given type: %s\n",
                 printtype (type));
        assert (0);
    }
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
    {
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
    }
    return b;
}

TYPE
stmt_assign_ex (var_ref * a, var_ref * b)
{
    symtab *ptrA = NULL;
    symtab *ptrB = NULL;
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
            int offsetA = 0;
            int offsetB = 0;
            int arr_offset = 0;
            int reg;

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
            ptrA = lookup (a->name);
            assert (NULL != ptrA);
            offsetA = ptrA->offset;
            reg = get_reg (b);

            /* if it's null it's a constant */
            if (NULL != b->name)
            {
                ptrB = lookup (b->name);
                assert (NULL != ptrB);
                if (ARR_ == ptrB->type)
                {
                    /* FIXME: Only 1 d right now */
                    arr_offset = 4 * b->var_ref_u.arr_info->dim_limit[0];
                }
                else
                {
                    arr_offset = 0;
                }
                offsetB = ptrB->offset + arr_offset;
            }

            if (ptrA->scope > 0)
            {
                /*
                if (NULL != b->name)
                {
                    if (ptrB->scope > 0)
                    {
                        asm_out ("\tlw\t$%d, %d($fp)\t# line %d\n", reg,
                                 offsetB, linenumber);
                    }
                    else
                    {
                        asm_out ("\tlw\t$%d, _%s\t# line %d\n", reg, b->name,
                                 linenumber);
                    }
                }
                else if (0 == b->place)
                {
                    asm_out ("\tli\t$%d, %d\t# line %d\n", reg,
                             b->tmp_val_u.tmp_intval, linenumber);
                }
                */
                reg = asm_emit_load_int(reg, b);
                ptrA->place = reg;
                asm_out ("\tsw\t$%d, %d($fp)\t# line %d\n", reg, offsetA,
                         linenumber);
                ns_reg (reg);
            }
            else
            {
                if (NULL != b->name)
                {
                    /*  FIXME: I don't think this is right */
                    if (ptrB->scope > 0)
                    {
                        asm_out ("\tlw\t$%d, %d($fp)\t# line %d\n", reg,
                                 offsetB, linenumber);
                    }
                    else
                    {
                        asm_out ("\tlw\t$%d, _%s\t# line %d\n", reg, b->name,
                                 linenumber);
                    }
                }
                else if (1 == b->is_return)
                {
                    reg = b->place;
                }
                else if (0 == b->place)
                {
                    asm_out ("\tli\t$%d, %d\t# line %d\n", reg,
                             b->tmp_val_u.tmp_intval, linenumber);
                }
                ptrA->place = reg;
                asm_out ("\tsw\t$%d, _%s\t# line %d\n", reg, a->name,
                         linenumber);
                ns_reg (reg);
            }
            break;
        case FLOAT_:
            ptrA = lookup (a->name);
            assert (NULL != ptrA);
            offsetA = ptrA->offset;
            reg = get_reg (b);

            /* if it's null it's a constant */
            if (NULL != b->name)
            {
                ptrB = lookup (b->name);
                assert (NULL != ptrB);
                if (ARR_ == ptrB->type)
                {
                    /* FIXME: Only 1 d right now */
                    arr_offset = 4 * b->var_ref_u.arr_info->dim_limit[0];
                }
                offsetB = ptrB->offset + arr_offset;
            }

            if (ptrA->scope > 0)
            {
                /* constant */
                if (NULL != b->name)
                {
                    /*asm_out ("\tlw\t$%d, %d($fp)\t# line %d\n", reg, offsetB, linenumber); */

                    asm_out ("\tl.s\t$f%d, _%s_%d\t# line %d\n", reg, b->name,
                             cur_const_val, linenumber);
                    frame_data_out ("\t_%s_%d: .float %f\t# line %d\n",
                                    b->name, cur_const_val,
                                    b->tmp_val_u.tmp_fval, linenumber);
                    cur_const_val++;
                }
                else if (0 <= b->place && REG_COUNT > b->place)
                {
                    /*fprintf(stderr, "line: %d b->tmp: %f\tb->place: %d\n", linenumber, b->tmp_val_u.tmp_fval, b->place); */
                    /* 
                     * if 0 <= place < REG_COUNT, then it was stored in $fN 
                     * so we need to store from $fN
                     */
                    if (0.0 == b->tmp_val_u.tmp_fval && b->place == 0)
                    {
                        /* if tmp isn't set, it's a read */
                        /*asm_out("\tmfc1\t$%d, $f%d\t# line %d\n", reg, b->place, linenumber); */
                        reg = 0;
                    }
                    else if (0.0 != b->tmp_val_u.tmp_fval && b->place == 0)
                    {
                        asm_out ("\tl.s\t$f%d, _f_%d\t# line %d\n", reg,
                                 cur_const_val, linenumber);
                        frame_data_out ("\t_f_%d: .float %f\t# line %d\n",
                                        cur_const_val, b->tmp_val_u.tmp_fval,
                                        linenumber);
                        cur_const_val++;
                    }
                    else
                    {
                        reg = b->place;
                    }
                }
                ptrA->place = reg;
                asm_out ("\ts.s\t$f%d, %d($fp)\t# line %d\n", reg,
                         ptrA->offset, linenumber);
                ns_reg (reg);
            }
            else
            {
                if (NULL != b->name)
                {
                    /* asm_out ("\tlw\t$%d, _%s\t# line %d\n", reg, b->name, linenumber); */
                    asm_out ("\tl.s\t$f%d, _%s_%d\t# line %d\n", reg, b->name,
                             cur_const_val, linenumber);
                    frame_data_out ("\t_%s_%d: .float %f\t# line %d\n",
                                    b->name, cur_const_val,
                                    b->tmp_val_u.tmp_fval, linenumber);
                    cur_const_val++;
                }
                else if (0 == b->place)
                {
                    /* FIXME: cant be li, needs to load from a static float in .data */
                    asm_out ("\tl.s\t$f%d, _%s\t# line %d\n", reg, b->name,
                             linenumber);
                }
                ptrA->place = reg;
                asm_out ("\tsw\t$f%d, _%s\t# line %d\n", reg, a->name,
                         linenumber);
                ns_reg (reg);
            }
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

id_list *
reverse_id_list (id_list * p)
{
    id_list *pr = NULL;
    while (p != NULL)
    {
        id_list *tmp = p->next;
        p->next = pr;
        pr = p;
        p = tmp;
    }
    return pr;
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
             ARR_) ? b->P_ini_i->init_id_u.P_arr_s->name : b->P_ini_i->
            init_id_u.name;
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
                 ARR_) ? b->P_ini_i->init_id_u.P_arr_s->name : b->P_ini_i->
                init_id_u.name;
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

symtab *
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
    {
        PST = insert (a, b, c, d);
    }
    return PST;
}

/* FIXME: fix the offsets here, don't handle variable length records */
TYPE
decl_enter_ST (var_decl * a)
{
    init_id *PII;
    id_list *PIL;
    symtab *symptr = NULL;
    TYPE ret = ZERO_;
    PIL = a->P_id_l;

    if (a == NULL)
        return ERROR_;

    if (a->type == ERROR_)
        return ERROR_;

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
                symptr = chk_insert (PII->init_id_u.P_arr_s->name, ARR_,
                                     PII->init_id_u.P_arr_s->arr_info, 0);
                symptr->offset = PII->offset;
            }
            else
            {
                autocast (a->type, PII);
                symptr = chk_insert (PII->init_id_u.name, INT_, PII, 0);
                symptr->offset = PII->offset;
            }
            break;
        case FLOAT_:
            if (PII->type == ARR_)
            {
                PII->init_id_u.P_arr_s->arr_info->arrtype = FLOAT_;
                symptr = chk_insert (PII->init_id_u.P_arr_s->name, ARR_,
                                     PII->init_id_u.P_arr_s->arr_info, 0);
                symptr->offset = PII->offset;
            }
            else
            {
                autocast (a->type, PII);
                symptr = chk_insert (PII->init_id_u.name, FLOAT_, PII, 0);
                symptr->offset = PII->offset;
            }
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
                      lookup (PSS1->struct_semantic_u.str_info.
                              struct_type_name)) == NULL)
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
    symtab *param;
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
        {
            ret = ERROR_;
        }
        else
        {
            /* FIXME: handle arrays and structs */
            param = lookup (c->PPAR->name);
            if (NULL != param)
            {
                param->offset = param_offset;
                param_offset += 4;
            }
        }
        c = c->next;
    }
    param_offset = 8;
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

/*
 * Here is where we save caller registers
 * as well as set up jump routines and such 
 * for calling a function
 */
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
    else if (0 == strcmp (a, "read"))
    {
        asm_emit_read ();
        PVR->place = 2;
        PVR->type = INT_;
    }
    else if (0 == strcmp (a, "fread"))
    {

        asm_emit_fread ();
        PVR->place = 0;
        PVR->type = FLOAT_;
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

void
asm_out (char const *fmt, ...)
{
    va_list ap;
    assert (NULL != fmt);
    va_start (ap, fmt);
    vfprintf (asm_out_fp, fmt, ap);
}

/* Unless global declarations exist, this function is not called. */
void
asm_emit_global_decls_start (void)
{
    asm_out ("# 'line #' comments printed at line endings correspond to\n");
    asm_out ("# the location in the original C-- source code when\n");
    asm_out ("# the line of MIPS assembly code was generated\n\n");

    asm_out (".data\t# line %d\n", linenumber);
}

void
asm_emit_global_decl_list (var_decl * a)
{
    init_id *PII = NULL;
    id_list *PIL = NULL;

    assert (NULL != a);

    PIL = a->P_id_l;
    assert (NULL != PIL);

    do
    {
        PII = PIL->P_ini_i;
        assert (NULL != PII);

        if (INT_ == a->type)
        {
            asm_out ("\t_%s: .word", PII->init_id_u.name);
        }
        else if (FLOAT_ == a->type)
        {
            asm_out ("\t_%s: .float", PII->init_id_u.name);
        }
        else
        {
            /* FIXME: asm_emit_global_decl_list only supports scalar values */
        }

        if (PII->assignment_during_initialization)
        {
            if (INT_ == a->type)
            {
                asm_out (" %d\t# line %d\n", PII->val_u.intval, linenumber);
            }
            else if (FLOAT_ == a->type)
            {
                asm_out (" %f\t# line %d\n", PII->val_u.fval, linenumber);
            }
        }
        else
        {
            /* If no value is specified, _always_ initialize static data to
             * zero (as recommended by Dr. Hsu) */
            if (INT_ == a->type)
            {
                PII->val_u.intval = 0;
            }
            else if (FLOAT_ == a->type)
            {
                PII->val_u.fval = 0.0;
            }
            asm_out (" 0\t# line %d\n", linenumber);
        }
    }
    while ((PIL = PIL->next));
}

int
set_var_decl_list_offsets (var_decl * v, int offset)
{

    init_id *PII = NULL;
    id_list *PIL = NULL;
    int i = 0;

    assert (NULL != v);

    PIL = v->P_id_l;
    assert (NULL != PIL);

    do
    {
        PII = PIL->P_ini_i;
        assert (NULL != PII);

        if (INT_ == v->type || FLOAT_ == v->type)
        {
            /*
               PII->offset = offset;
               offset -= 4;
             */
            if (ARR_ == PII->type)
            {
                offset += 4;    /* to remove prior -4 */
                /* FIXME: Handle variable length structures */
                /* FIXME: going to need something trickier for multi dimensional */
                for (i = 0; i < PII->init_id_u.P_arr_s->arr_info->dim; i++)
                {
                    offset -=
                        (4 * PII->init_id_u.P_arr_s->arr_info->dim_limit[i]);
                }
            }
            else
            {
                /* need to multiply by the size of the struct */
            }
            PII->offset = offset;
            offset -= 4;
        }
        else if (STR_VAR_ == v->type)
        {
        }
    }
    while ((PIL = PIL->next));

    return offset;
}

void
asm_emit_scoped_decl_list (var_decl * v)
{
    init_id *PII = NULL;
    id_list *PIL = NULL;

    assert (NULL != v);

    PIL = v->P_id_l;
    assert (NULL != PIL);

    do
    {
        PII = PIL->P_ini_i;
        assert (NULL != PII);

        if (PII->assignment_during_initialization)
        {
            if (INT_ == v->type)
            {
                int reg = get_result_reg ();
                asm_out ("\tli\t$%d, %d\t# line %d\n", reg, PII->val_u.intval,
                         linenumber);
                asm_out ("\tsw\t$%d, %d($fp)\t# line %d\n", reg, PII->offset,
                         linenumber);
            }
            else if (FLOAT_ == v->type)
            {
                int reg = get_result_reg ();
                /* asm_out ("\tli\t$%d, %f\t# line %d\n", reg, PII->val_u.fval, linenumber); */
                asm_out ("\tla\t$%d, _%s_%d\t# line %d\n", reg,
                         PII->init_id_u.name, cur_const_val, linenumber);
                frame_data_out ("\t_%s_%d:\t.float\t%f\t# line %d\n",
                                PII->init_id_u.name, cur_const_val,
                                PII->val_u.fval, linenumber);
                cur_const_val++;
                asm_out ("\tsw\t$%d, %d($fp)\t# line %d\n", reg, PII->offset,
                         linenumber);
            }
            else
            {
                /* FIXME: asm_emit_global_decl_list only supports scalar values */
            }
        }
        else
        {
            if (INT_ == v->type)
            {
                PII->val_u.intval = 0;
            }
            else if (FLOAT_ == v->type)
            {
                PII->val_u.fval = 0.0;
            }
        }
    }
    while ((PIL = PIL->next));
}

/* Return value is the register holding the result of the expr */
int
asm_emit_relop_factor (var_ref * a, var_ref * b, int opval)
{
    int regA = get_reg (a);
    int regB = -9999;
    int res_reg;

    if (NULL == b)
    {
        res_reg = regA;
    }
    else
    {
        res_reg = get_result_reg ();
        regB = get_reg (b);
    }

    /* TODO: the parser enforces identical typing? */
    if (INT_ == a->type)
    {
        regA = asm_emit_load_int(regA, a);

        /* if b is null, this is an expr-to-relop_factor reduction */
        if (NULL != b)
        {
            regB = asm_emit_load_int(regB, b);
        }
    }
    else
    { /* FLOAT_ */
        regA = asm_emit_load_float(regA, a);

        /* if b is null, this is an expr-to-relop_factor reduction */
        if (NULL != b)
        {
            regB = asm_emit_load_float(regB, b);
        }
    }

    /* TODO: emit asm for comparisons */

    if (NULL != b)
    {
        switch (opval)
        {
        case OP_GT:
            asm_out ("\tsgt\t$%d, $%d, $%d\t# line %d\n", res_reg, regA, regB,
                     linenumber);
            break;
        case OP_GE:
            asm_out ("\tsge\t$%d, $%d, $%d\t# line %d\n", res_reg, regA, regB,
                     linenumber);
            break;
        case OP_LT:
            asm_out ("\tslt\t$%d, $%d, $%d\t# line %d\n", res_reg, regA, regB,
                     linenumber);
            break;
        case OP_LE:
            asm_out ("\tsle\t$%d, $%d, $%d\t# line %d\n", res_reg, regA, regB,
                     linenumber);
            break;
        case OP_NE:
            asm_out ("\tsne\t$%d, $%d, $%d\t# line %d\n", res_reg, regA, regB,
                     linenumber);
            break;
        case OP_EQ:
            asm_out ("\tseq\t$%d, $%d, $%d\t# line %d\n", res_reg, regA, regB,
                     linenumber);
            break;
        default:
            fprintf (stderr, "don't know how to handle relop: %d\n", opval);
            assert (0);
        }
    }


    free_reg (regA);
    if (-9999 != regB)
        free_reg (regB);
    save_reg (res_reg);

    return res_reg;
}

/* Return value is the register holding the result of the expr */
int
asm_emit_expr (var_ref * a, var_ref * b, int opval)
{

    int regA = get_reg (a);
    int regB = get_reg (b);
    int res_reg = get_result_reg ();

    /* TODO: the parser enforces identical typing? */
    if (INT_ == a->type)
    {
        regA = asm_emit_load_int(regA, a);

        if (regB != regA)
        {
            regB = asm_emit_load_int(regB, b);
        }
    }
    else
    {   /* FLOAT_ */
        regA = asm_emit_load_float(regA, a);
        if(regB != regA){
            regB = asm_emit_load_float(regB, b);
        }
    }

    if (OP_PLUS == opval)
    {
        if (INT_ == a->type)
        {
            asm_out ("\tadd\t$%d, $%d, $%d\t# line %d\n", res_reg, regA, regB,
                     linenumber);
        }
        else
        {
            asm_out ("\tadd.s\t$f%d, $f%d, $f%d\t# line %d\n", res_reg, regA,
                     regB, linenumber);
        }
    }
    else if (OP_MINUS == opval)
    {
        if (INT_ == a->type)
        {
            asm_out ("\tsub\t$%d, $%d, $%d\t# line %d\n", res_reg, regA, regB,
                     linenumber);
        }
        else
        {
            asm_out ("\tsub.s\t$f%d, $f%d, $f%d\t# line %d\n", res_reg, regA,
                     regB, linenumber);
        }
    }

    free_reg (regA);
    free_reg (regB);
    save_reg (res_reg);
    return res_reg;
}

/* Return value is the register holding the result of the expr */
int
asm_emit_term (var_ref * a, var_ref * b, int opval)
{

    int regA = get_reg (a);
    int regB = get_reg (b);
    int res_reg = get_result_reg ();

    /* TODO: the parser enforces identical typing? */
    if (INT_ == a->type)
    {
        regA = asm_emit_load_int(regA, a);

        if (regB != regA)
        {
            regB = asm_emit_load_int(regB, b);
        }
    }
    else
    { /* FLOAT_ */
        regA = asm_emit_load_float(regA, a);

        if (regB != regA)
        {
            regB = asm_emit_load_float(regB, b);
        }
    }

    if (OP_TIMES == opval)
    {
        if (INT_ == a->type)
        {
            asm_out ("\tmul\t$%d, $%d, $%d\t# line %d\n", res_reg, regA, regB,
                     linenumber);
        }
        else
        {
            asm_out ("\tmul.s\t$f%d, $f%d, $f%d\t# line %d\n", res_reg, regA,
                     regB, linenumber);
        }
    }
    else if (OP_DIVIDE == opval)
    {
        if (INT_ == a->type)
        {
            asm_out ("\tdiv\t$%d, $%d, $%d\t# line %d\n", res_reg, regA, regB,
                     linenumber);
        }
        else
        {
            asm_out ("\tdiv.s\t$f%d, $f%d, $f%d\t# line %d\n", res_reg, regA,
                     regB, linenumber);
        }
    }

    free_reg (regA);
    free_reg (regB);
    save_reg (res_reg);
    return res_reg;
}

/* returns register */
int
asm_emit_load_int(int reg, var_ref* v){
    symtab* ptr = NULL;

    if (NULL != v->name)
    {
        ptr = lookup (v->name);
        assert (NULL != ptr);

        /*  FIXME: I don't think this is right */
        if (ptr->scope > 0)
        {
            asm_out ("\tlw\t$%d, %d($fp)\t# line %d\n", reg,
                     ptr->offset, linenumber);
        }
        else
        {
            asm_out ("\tlw\t$%d, _%s\t# line %d\n", reg, v->name,
                     linenumber);
        }
    }
    else
    {
        if (0 == v->place)
        {
            asm_out ("\tli\t$%d, %d\t# line %d\n", reg,
                     v->tmp_val_u.tmp_intval, linenumber);
        }
        else if (1 == v->is_return)
        {
            reg = v->place;
        }
        else
        {
            reg = v->place;
        }
    }

    return reg;
}

int
asm_emit_load_float(int reg, var_ref* v){
    symtab* ptr = NULL;

    /* if it's null it's a constant */
    if (NULL != v->name)
    {
        ptr = lookup (v->name);
        assert (NULL != ptr);

        /*  FIXME: I don't think this is right */
        if (ptr->scope > 0)
        {
            asm_out ("\tl.s\t$f%d, %d($fp)\t# line %d\n", reg,
                     ptr->offset, linenumber);
        }
        else if (1 == v->is_return)
        {
            reg = v->place;
        }
        else if (0 == ptr->place)
        {
            asm_out ("\tl.s\t$f%d, _%s\t# line %d\n", reg, v->name,
                     linenumber);
        }
    }
    else
    {
        /*asm_out ("\tlw\t$%d, _%s\t# line %d\n", regB, b->name, linenumber); */
        asm_out ("\tl.s\t$f%d, _f_%d\t# line %d\n", reg, cur_const_val,
                 linenumber);
        frame_data_out ("\t_f_%d: .float %f\t# line %d\n", cur_const_val,
                        v->tmp_val_u.tmp_fval, linenumber);
        cur_const_val++;
    }

    return reg;
}

void
asm_emit_write (TypeList * idl)
{
    symtab *symptr = NULL;
    if (NULL != idl->P_var_r->name)
    {
        symptr = lookup (idl->P_var_r->name);
    }
    if (INT_ == idl->P_var_r->type)
    {
        asm_out ("\tli\t$v0, 1\n");
        if (NULL != symptr)
        {
            if (symptr->scope > 0)
            {
                asm_out ("\tlw\t$a0, %d($fp)\n", symptr->offset);
            }
            else
            {
                asm_out ("\tlw\t$a0, _%s\n", idl->P_var_r->name);
            }
        }
        else
        {
            /* FIXME: I don't think this is right */
            asm_out ("\tmove\t$a0, $%d\n", idl->P_var_r->place);
        }
    }
    else if (FLOAT_ == idl->P_var_r->type)
    {
        asm_out ("\tli\t$v0, 2\n");
        if (NULL != symptr)
        {
            asm_out ("\tl.s\t$f12, %d($fp)\n", symptr->offset);
        }
        else
        {
            /* FIXME: I don't think this is right either */
            asm_out ("\tmove.s\t$f12, $%d\n", idl->P_var_r->place);
        }
    }
    else
    {                           /* string */
        asm_out ("\tli\t$v0, 4\n");
        /* this might require scope? */
        frame_data_out ("\t_sConst%d: .asciiz %s\n", cur_const_val,
                        idl->P_var_r->tmp_val_u.tmp_str);
        asm_out ("\tla\t$a0, _sConst%d\n", cur_const_val);
        cur_const_val++;
    }
    asm_out ("\tsyscall\n");
}

void
asm_emit_read ()
{
    asm_out ("\tli\t$v0, 5\n");
    asm_out ("\tsyscall\n");
}

void
asm_emit_fread ()
{
    asm_out ("\tli\t$v0, 6\n");
    asm_out ("\tsyscall\n");
}

void
gen_prologue (const char *name)
{
    int i;
    asm_out (".text\n");
    asm_out ("%s:\n", name);

    /*
       asm_out ("\tsub\t$sp, $sp, 4\n");
     */
    asm_out ("\tsw\t$ra, ($sp)\n");

    asm_out ("\tsw\t$fp, -4($sp)\n");
    asm_out ("\tadd\t$fp, $sp, -4\n");
    asm_out ("\tadd\t$sp, $sp, -8\n");
    asm_out ("\tlw\t$v0, _framesize_%s\n", name);
    asm_out ("\tsub\t$sp, $sp, $v0\n");

    /* save $s0-7 */
    if (0 != strcmp (name, "main"))
    {
        for (i = 0; i < 8; i++)
        {
            asm_out ("\tsub\t$sp, $sp, 4\t#push $s%d\n", i);
            asm_out ("\tsw\t$s%d, ($sp)\n", i);
        }
    }

    asm_out ("\n");

    asm_out ("_begin_%s:\n", name);
}

void
gen_epilogue (const char *name)
{
    int i;
    asm_out ("\n_end_%s:\t# line %d\n", name, linenumber);

    /* restore $s0-7 in reverse order (due to stack) */
    if (0 != strcmp (name, "main"))
    {
        for (i = 7; i >= 0; i--)
        {
            asm_out ("\tlw\t$s%d, ($sp)\t#pop $s%d\n", i, i);
            asm_out ("\tadd\t$sp, $sp, 4\n", i);
        }
    }

    asm_out ("\tlw\t$ra, 4($fp)\n");
    asm_out ("\tadd\t$sp, $fp, 4\n");
    asm_out ("\tlw\t$fp, 0($fp)\n");

    if (strcmp (name, "main") == 0)
    {
        asm_out ("\tli\t$v0, 10\n");
        asm_out ("\tsyscall\n");
    }
    else
    {
        asm_out ("\tjr\t$ra\n");
    }

    asm_out ("\n.data\n");
    asm_out ("\t_framesize_%s: .word %d\n", name, abs (offset) - 4);    /* -4 to remove prior +4 */
    if (NULL != frame_data)
    {
        asm_out (frame_data);
        free (frame_data);
        frame_data = NULL;
    }
}

int
get_offset (char *name)
{
    symtab *ptr = lookup (name);

    assert (NULL != ptr);

    if (INT_ == ptr->type)
    {
        return ptr->offset;

    }
    else if (ARR_ == ptr->type)
    {

    }

    return ptr->offset;
}

void
gen_control_start (int test_label_num)
{
    asm_out ("_Test%d:\t# line %d\n", test_label_num, linenumber);
}

void
gen_control_test (var_ref * a, int exit_label_num)
{
    if (NULL == a->name)
    {
        int reg = get_reg (NULL);
        switch (a->type)
        {
        case INT_:
            if(1 > a->place || REG_COUNT <= a->place){
                asm_out ("\tli\t$%d, %d\t# line %d\n", reg,
                     a->tmp_val_u.tmp_intval, linenumber);
            }else{
                reg = a->place;
            }
            break;
        case FLOAT_:
            frame_data_out ("\t_sConst%d: .float %f\t# line %d\n",
                            cur_const_val, a->tmp_val_u.tmp_fval, linenumber);
            asm_out ("\tla\t$%d, _sConst%d\n", reg, cur_const_val);
            cur_const_val++;
            break;
        default:
            assert (0);
        }
        asm_out ("\tbeqz\t$%d, _Lexit%d\n", reg, exit_label_num);
    }
    else if (a->place > 0 && a->place < REG_COUNT)     /* FIXME: this is a bad way to check if place is initialized */
    {
        int testreg = get_reg (NULL);
        asm_out ("\tmove\t$%d, $%d\t# line %d\n", testreg, a->place,
                 linenumber);
        asm_out ("\tbeqz\t$%d, _Lexit%d\n", testreg, exit_label_num);
    }
    else
    {
        asm_out ("\tli\t$%d, %d\t# line %d\n", a->place,
                 a->tmp_val_u.tmp_intval, linenumber);
        asm_out ("\tbeqz\t$%d, _Lexit%d\n", a->place, exit_label_num);
    }
}

void
gen_control_iterate (int label_num)
{
    asm_out ("\tj\t_Test%d\t# line %d\n", label_num, linenumber);
}

void
gen_control_endlabel (int label_num)
{
    asm_out ("_Lexit%d:\t# line %d\n", label_num, linenumber);
}

void
gen_control_ifelse_endlabel (int label_num)
{
    asm_out ("_Lelse%d:\t# line %d\n", label_num, linenumber);
}

void
gen_control_exit_ifelse (int label_num)
{
    asm_out ("\tj\t_Lelse%d\t# line %d\n", label_num, linenumber);
}

void
frame_data_out (char const *fmt, ...)
{

    va_list ap;
    assert (NULL != fmt);
    va_start (ap, fmt);
    if (frame_data == NULL)
    {
        frame_data = malloc (sizeof (char) * 128);
    }
    else
    {
        frame_data =
            realloc (frame_data, strlen (frame_data) + sizeof (char) * 128);
    }
    vsprintf (&(frame_data[strlen (frame_data)]), fmt, ap);
    /* This removes extra allocated memory */
    frame_data = realloc (frame_data, strlen (frame_data) + 1);
}

/*
vim: expandtab shiftwidth=4 tabstop=4 smarttab
*/
