#ifndef SYMTAB_H
#define SYMTAB_H

#include "ourtypes.h"

void init_sym_table (void);
void destroy_sym_table (void);
semrec_t *putsym (semrec_t * ptr);
semrec_t *getsym (char const *, int scope);
semrec_t *new_semrec (char const *sym_name);
void putsymlist (semrec_t * item);
int list_length (semrec_t * list);
void dump_symtab (void);

void apply_type(semrec_t* list, type_t type);
void apply_scope(semrec_t* list, int scope);

/* removes all elements in scope from the symbol table */
void free_scope(int scope);

#endif

/*
vim: expandtab shiftwidth=4 tabstop=4 smarttab
*/
