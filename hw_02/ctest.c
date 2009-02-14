/* unit tests for C code */

#include <assert.h>
#include <list.h>
#include <stdlib.h>

void
destroy_item_func(Item *item)
{
}

int
main (void)
{
    List *list = init_list();
    destroy_list(list, destroy_item_func);
    return EXIT_SUCCESS;
}
