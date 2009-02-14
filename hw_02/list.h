#ifndef LIST_H
#define LIST_H

#include <global.h>

typedef struct item_s
{
    struct item_s *next;
    void *data;
}
Item;

typedef struct list_s
{
    Item *first;
    Item *last;
    Item *current;
}
List;

/**
 * Append a new item to the end of the list.
 */
void append(List *list, Item *item);

/**
 * Return the next node in the list.
 */
Item * next_item(List *list);

Bool has_next(List *list);

/**
 * Must call this on a list before usage. list must point to NULL.
 */
List * init_list(void);

/**
 * Must call this on a list when done using list.
 */
void destroy_list(List *list, void (*destroy_item_func)(Item *item));

#endif /* ifndef LIST_H */
