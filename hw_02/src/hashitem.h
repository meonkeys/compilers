#ifndef HASHITEM_H
#define HASHITEM_H

#include <global.h>
#include <string.h>

/* Not including null character. Specified in assignment description. */
#define MAX_ID_LENGTH 256

typedef struct hash_key_s
{
    char id[MAX_ID_LENGTH + 1];
}
HashKey;

typedef struct hash_value_s
{
    int count;
}
HashValue;

/* TODO:
#include <hashtable.h>
DEFINE_HASHTABLE_INSERT(ht_insert, HashKey, HashValue);
DEFINE_HASHTABLE_SEARCH(ht_search, HashKey, HashValue);
*/

#endif
