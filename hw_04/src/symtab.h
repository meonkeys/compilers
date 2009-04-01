#ifndef SYMTAB_H
#define SYMTAB_H

#include "ourtypes.h"

void init_sym_table (void);
void destroy_sym_table (void);
semrec_t *putsym (char const *, int);
semrec_t *getsym (char const *);
void dump_symtab (void);

#endif
