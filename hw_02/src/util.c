/* System headers */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>             /* for memcmp() */

/* Custom headers */
#include <util.h>

void
display_comments (List * comments, FILE * inputStream)
{
    while (has_next (comments))
    {
        ListItem *next = next_item (comments);
        CommentPos *comment_pos = (CommentPos *) next->data;
        long left = comment_pos->end - comment_pos->start;
        int current = EOF;
        assert (0 == fseek (inputStream, comment_pos->start, SEEK_SET));
        do
        {
            current = getc (inputStream);
            assert (EOF != current);
            assert (EOF != putchar (current));
            left -= 1;
        }
        while (left > 0);
        assert (EOF != putchar ('\n'));
    }
}


unsigned int
ELFHash (void *val)
{
    char *str = (char *) val;
    unsigned int hash = 0;
    unsigned int x = 0;
    unsigned int i = 0;
    unsigned int len = strlen (str);

    for (i = 0; i < len; str++, i++)
    {
        hash = (hash << 4) + (*str);
        if ((x = hash & 0xF0000000L) != 0)
        {
            hash ^= (x >> 24);
        }
        hash &= ~x;
    }

    return hash;
}

int
equalkeys (void *k1, void *k2)
{
    return (0 == memcmp (k1, k2, sizeof (HashKey)));
}

static int
compare (const void *hk1, const void *hk2)
{
    char *id1 = (char *) ((HashKey *) hk1)->id;
    char *id2 = (char *) ((HashKey *) hk2)->id;

    return strcmp (id1, id2);
}

void
display_hashtable (struct hashtable *ht)
{
    int i = 0;
    struct hashtable_itr *itr = NULL;
    HashKey *keys;
    unsigned int size = hashtable_count (ht);

    if (size <= 0)
        return;

    keys = malloc (size * sizeof (HashKey));
    assert (NULL != keys);

    itr = hashtable_iterator (ht);
    do
    {
        HashKey *k = hashtable_iterator_key (itr);

        memset (keys[i].id, '\0', MAX_ID_LENGTH + 1);
        strncpy (keys[i].id, k->id, MAX_ID_LENGTH);
        i++;
    }
    while (hashtable_iterator_advance (itr));

    free (itr);

    qsort (keys, size, sizeof (HashKey), compare);

    for (i = 0; i < size; i++)
    {
        HashValue *v = hashtable_search (ht, &keys[i]);
        assert (NULL != v);
        printf ("%s %d\n", keys[i].id, v->count);
    }

    free (keys);
}
