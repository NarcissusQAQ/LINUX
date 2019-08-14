#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_
#include <stdlib.h>
#include <string.h>
typedef struct node{
	char data[128];
	struct node*next;
	struct node *prev;
}Node;

typedef struct list{
	Node* first;
	Node* last;
}List;

List *Creat_linkedlist();
void add_a_node(List* list,char *s);

#endif