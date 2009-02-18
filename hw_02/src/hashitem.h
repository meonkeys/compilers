#ifndef HASHITEM_H
#define HASHITEM_H

#include <global.h>
#include <string.h>

typedef struct hash_key_s
{
    unsigned int key;
}
HashKey;

typedef struct hash_value_s
{
    char* id;
    int count;
}
HashValue;

#endif
