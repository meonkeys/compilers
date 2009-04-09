#include <stdlib.h>
#include <header.h>


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
    }
}
