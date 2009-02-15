#ifndef LIST_H
#define LIST_H

#include <global.h>

typedef struct item_s
{
    struct item_s *next;
    void *data;
}
ListItem;

typedef struct list_s
{
    ListItem *first;
    ListItem *last;             /* guarantees O(1) inserts */
    ListItem *current;          /* iterator-like functionality */
    long size;
}
List;

/**
 * Append a new item to the end of the list.
 */
void append (List * list, ListItem * item);

/**
 * Return the next node in the list.
 */
ListItem *next_item (List * list);

Bool has_next (List * list);

/**
 * Must call this on a list before usage.
 */
List *init_list (void);

/**
 * Must call this on an item before usage.
 */
ListItem *init_item (void *data);

/**
 * Must call this on a list when done using list.
 */
void destroy_list (List * list, void (*destroy_data) (void *data));

#endif /* ifndef GLOBAL_H */
