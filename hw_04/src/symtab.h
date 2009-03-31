#ifndef SYMTAB_H
#define SYMTAB_H

#include "ourtypes.h"

void init_sym_table (void);
void destroy_sym_table (void);
symrec_t *putsym (char const *, int);
symrec_t *getsym (char const *);
void dump_symtab (void);

#endif
