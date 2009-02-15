/* unit tests for C list code */

#include <assert.h>
#include <list.h>
#include <stdlib.h>
#include <string.h>

/* not counting the null character ending the string */
#define MAX_NAME_LENGTH 255

typedef struct mydata_s
{
    int id;
    char name[MAX_NAME_LENGTH + 1];
} MyData;

void
destroy_data(void *data)
{
    free(data);
}

MyData *
init_MyData (int id, char *name, size_t name_length)
{
    MyData *new = NULL;
    assert(name_length < MAX_NAME_LENGTH);
    new = malloc(sizeof(MyData));
    assert(NULL != new);
    new->id = id;
    memset(new->name, '\0', MAX_NAME_LENGTH);
    strncpy(new->name, name, name_length);
    return new;
}

int
main (void)
{
    /* declarations */
    MyData *mydata1;
    MyData *mydata2;
    Item *item1 = NULL, *item2 = NULL, *tmp_item = NULL;
    List *list = init_list();

    /* tests */
    assert(NULL != list);
    mydata1 = init_MyData(1, "Joe Smith", strlen("Joe Smith"));
    item1 = init_item(mydata1);
    assert(NULL != item1);
    assert(mydata1 == item1->data);
    assert(1 == ((MyData *)item1->data)->id);
    assert(strcmp(((MyData *)item1->data)->name, "Joe Smith") == 0);
    item1 = init_item(mydata1);
    mydata2 = init_MyData(2, "Jim Smythe", strlen("Joe Smythe"));
    item2 = init_item(&mydata2);
    assert(FALSE == has_next(list));
    append(list, item1);
    assert(TRUE == has_next(list));
    append(list, item2);
    tmp_item = next_item(list);
    assert(tmp_item == item1);
    tmp_item = next_item(list);
    assert(tmp_item == item2);
    assert(FALSE == has_next(list));
    assert(2 == list->size);

    /* cleanup */
    destroy_list(list, destroy_data);
    list = NULL;

    return EXIT_SUCCESS;
}
