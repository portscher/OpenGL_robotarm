/******************************************************************
*
* List.h
*
* Description: Code providing handling of lists.  
* Courtesy of http://www.kixor.net
*
* Interactive Graphics and Simulation Group
* Institute of Computer Science
* University of Innsbruck
*
*******************************************************************/

#ifndef __LIST_H
#define __LIST_H

typedef struct
{
	int item_count;
	int current_max_size;
	char growable;

	void **items;
	char **names;	
} list;

void list_make(list *listo, int size, char growable);
int list_add_item(list *listo, void *item, char *name);
int list_find(list *listo, char *name_to_find);
void list_delete_index(list *listo, int indx);
void list_delete_all(list *listo);
void list_free(list *listo);

void test_list();
#endif
