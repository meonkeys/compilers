#ifndef UTIL_H
#define UTIL_H

#include <ourtypes.h>

void our_free (semrec_t * bt);

void our_free_list (semrec_t * rec);

truth_t typecmp (type_t t1, type_t t2);


#endif

/*
vim: expandtab shiftwidth=4 tabstop=4 smarttab
*/
