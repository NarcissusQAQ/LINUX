#include "linkedlist.h"

/*
	创建一条链表，返回头结点
*/
List* Creat_linkedlist()
{
	List *list = (List*)malloc(sizeof(List));
	list->first=list->last= NULL;
	return list;
}

/*
	增加数据节点
*/
void add_a_node(List* list,char* s)
{
	//step1:开辟空间
	Node *p = (Node*)malloc(sizeof(Node));
	p->next = p->prev = NULL;
	strcpy(p->data,s);
	
	//step2:把新的数据节点插入到链表中
	if(list->first == NULL)
	{
		list->first = list->last = p;
		p->next=list->first;
		p->prev=list->first;
		list->first->prev=p;
		list->first->next=p;
		
	}
	else
	{
		p->prev = list->last; //新结点的上一个是原尾巴
		list->last->next = p; //原尾巴的下一个是新节点 
		list->last = p; //新节点成为新的尾巴 
		p->next=list->first;
		list->first->prev=p;
	}
}


