#include <assert.h>
#include <list.h>
#include <stdlib.h>

void
append (List * list, ListItem * item)
{
    assert (NULL != list);
    assert (NULL != item);
    if (NULL == list->first)
    {
        assert (NULL == item->next);
        list->first = item;
        list->last = item;
        list->current = item;
    }
    else
    {
        list->last->next = item;
        list->last = list->last->next;
    }
    list->size++;
}

ListItem *
next_item (List * list)
{
    ListItem *this;
    assert (NULL != list->current);
    this = list->current;
    list->current = list->current->next;
    return this;
}

Bool
has_next (List * list)
{
    assert (NULL != list);
    if (NULL == list->current)
        return FALSE;
    else
        return TRUE;
}

List *
init_list (void)
{
    List *list = malloc (sizeof (List));
    assert (NULL != list);
    list->first = NULL;
    list->last = NULL;
    list->current = NULL;
    list->size = 0;
    return list;
}

static void
reset (List * list)
{
    list->current = list->first;
}

void
destroy_list (List * list, void (*destroy_data) (void *data))
{
    reset (list);
    while (has_next (list))
    {
        ListItem *item = next_item (list);
        destroy_data (item->data);
        free (item);
    }
    free (list);
}


ListItem *
init_item (void *data)
{
    ListItem *item = malloc (sizeof (ListItem));
    assert (NULL != item);
    item->next = NULL;
    item->data = data;
    return item;
}
