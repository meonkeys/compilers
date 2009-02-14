#include <assert.h>
#include <list.h>
#include <stdlib.h>

void
append (List *list, Item *item)
{
    Item *n = NULL;
    assert(NULL != list);
    assert(NULL != item);
    /* TODO: implement */
    n = NULL;
}

Item *
next_item (List *list)
{
    Item *this;
    assert(NULL != list->current);
    this = list->current;
    list->current = list->current->next;
    return this;
}

Bool
has_next (List *list)
{
    assert(NULL != list);
    if (NULL == list->current)
        return FALSE;
    else
        return TRUE;
}

List *
init_list(void)
{
    List *list = malloc(sizeof(List));
    assert(NULL != list);
    list->first = NULL;
    list->last = NULL;
    list->current = NULL;
    return list;
}

void
destroy_list(List *list, void (*destroy_item_func)(Item *item))
{
    while (has_next(list))
    {
        Item *item = next_item(list);
        destroy_item_func(item);
        item = NULL;
    }
    free(list);
    list = NULL;
}
