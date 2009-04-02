#ifndef SYMTAB_H
#define SYMTAB_H

#include "ourtypes.h"

void init_sym_table (void);
void destroy_sym_table (void);
semrec_t *putsym (semrec_t * ptr);
semrec_t *getsym (char const *);
semrec_t *new_semrec (char const *sym_name);
void putsymlist (semrec_t * item, type_t type);
int list_length (semrec_t * list);
void dump_symtab (void);

#endif

/*
vim: expandtab shiftwidth=4 tabstop=4 smarttab
*/
