/* System headers */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>             /* for memcmp() */

/* Custom headers */
#include <util.h>

void
display_comments (List *comments, FILE *inputStream)
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

int
valcomp (const void *hv1, const void *hv2)
{
    HashValue *val1 = (HashValue *) hv1;
    HashValue *val2 = (HashValue *) hv2;

    int result = strcmp (val1->id, val2->id);
    if (result == 0)
    {
        return val1->count - val2->count;
    }
    return result;
}

void
display_hashtable (struct hashtable *ht)
{
    struct hashtable_itr *itr = NULL;
    HashValue *v = NULL;
    unsigned int size = hashtable_count (ht);

    HashValue *values = malloc (size * sizeof (HashValue));

    int i = 0;
    if (size > 0)
    {
        itr = hashtable_iterator (ht);
        do
        {
            v = hashtable_iterator_value (itr);

            values[i].id = v->id;
            values[i].count = v->count;
            i++;
        }
        while (hashtable_iterator_advance (itr));
    }
    free (itr);

    qsort (values, size, sizeof (HashValue), valcomp);

    for (i = 0; i < size; i++)
    {
        printf ("%s %d\n", values[i].id, values[i].count);
    }

    free (values);
}

/*
 * Add reserved words to the hashtable eventually
 */
