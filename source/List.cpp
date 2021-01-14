/******************************************************************
*
* List.c
*
* Description: Code providing handling of lists.  
* Courtesy of http://www.kixor.net
* 
* Interactive Graphics and Simulation Group
* Institute of Computer Science
* University of Innsbruck
*
*******************************************************************/

/* Standard includes */
#include <cstring>
#include <cstdlib>
#include <cstdio>

#include "List.h"


// internal helper functions
char list_is_full(list *listo)
{
    return (listo->item_count == listo->current_max_size);
}

void list_grow(list *old_listo)
{
    int i;
    list new_listo;

    list_make(&new_listo, old_listo->current_max_size * 2, old_listo->growable++);

    for (i = 0; i < old_listo->current_max_size; i++)
        list_add_item(&new_listo, old_listo->items[i], old_listo->names[i]);

    list_free(old_listo);

    //copy new structure to old list
    old_listo->names = new_listo.names;
    old_listo->items = new_listo.items;
    old_listo->item_count = new_listo.item_count;
    old_listo->current_max_size = new_listo.current_max_size;
    old_listo->growable = new_listo.growable;
}
//end helpers

void list_make(list *listo, int start_size, char growable)
{
    listo->names = (char **) malloc(sizeof(char *) * start_size);
    listo->items = (void **) malloc(sizeof(void *) * start_size);
    listo->item_count = 0;
    listo->current_max_size = start_size;
    listo->growable = growable;
}

int list_add_item(list *listo, void *item, char *name)
{
    int name_length;
    char *new_name;

    if (list_is_full(listo))
    {
        if (listo->growable)
            list_grow(listo);
        else
            return -1;
    }

    listo->names[listo->item_count] = nullptr;
    if (name != nullptr)
    {
        name_length = strlen(name);
        new_name = (char *) malloc(sizeof(char) * name_length + 1);
        strncpy(new_name, name, name_length);
        listo->names[listo->item_count] = new_name;
    }

    listo->items[listo->item_count] = item;
    listo->item_count++;

    return listo->item_count - 1;
}

int list_find(list *listo, char *name_to_find)
{
    for (int i = 0; i < listo->item_count; i++)
    {
        if (strncmp(listo->names[i], name_to_find, strlen(name_to_find)) == 0)
            return i;
    }

    return -1;
}

void list_delete_index(list *listo, int indx)
{
    //remove item
    if (listo->names[indx] != nullptr)
        free(listo->names[indx]);

    //restructure
    for (int j = indx; j < listo->item_count - 1; j++)
    {
        listo->names[j] = listo->names[j + 1];
        listo->items[j] = listo->items[j + 1];
    }

    listo->item_count--;
}

void list_delete_all(list *listo)
{
    int i;

    for (i = listo->item_count - 1; i >= 0; i--)
        list_delete_index(listo, i);
}

void list_free(list *listo)
{
    list_delete_all(listo);
    free(listo->names);
    free(listo->items);
}