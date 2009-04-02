#ifndef SYMTAB_H
#define SYMTAB_H

#include "ourtypes.h"

void init_sym_table (void);
void destroy_sym_table (void);
semrec_t *putsym (char const *, int);
semrec_t *getsym (char const *);
semrec_t *new_semrec (char const *sym_name);
semrec_t *putsymlist (semrec_t *item, type_t type);
void dump_symtab (void);

#endif
