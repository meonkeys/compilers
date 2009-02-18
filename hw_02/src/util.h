#ifndef UTIL_H
#define UTIL_H

/* System headers */
#include <stdlib.h>

/* Custom headers */
#include <global.h>
#include <hashtable.h>
#include <hashtable_itr.h>
#include <hashtable_utility.h>
#include <list.h>

typedef struct comment_pos_t
{
    long start;
    long end;
}
CommentPos;

void display_comments (List * comments, FILE * inputStream);
void display_hashtable (struct hashtable *ht);
unsigned int ELFHash (void *val);
int equalkeys (void *k1, void *k2);
int valcomp (const void *hv1, const void *hv2);

#endif
