/**
 *  Source file for a linked list in C
 *
 * @authors:   		Michael Denzel
 * @creation_date:	2016-09-05
 * @contact:		m.denzel@cs.bham.ac.uk
 */

//standard includes

// TODO: Add the includes you need
#include <stdio.h>
#include <stdlib.h>

//own includes
#include "linkedlist.h"


// TODO: Implement those methods!
int get(list * l, unsigned int index){
	list_node *node = l->start;
	for(int i = 0; i < index; i++) node = node->next;
	return node->value;
}

int prepend(list * l, int data){
	struct list_node *new = malloc(sizeof(struct list_node));
	if(new) {
		new->value = data;
		new->next = l->start;
		l->start = new;
		if(!l->end) l->end = new; //if empty list
		l->size+=1;
		return 0;
	} else return -1;
}

int append(list * l, int data){
	struct list_node *new = malloc(sizeof(struct list_node));
	if(new) {
		new->value = data;
		new->next = NULL;
		l->end->next = new;
		l->end = new;
		l->size+=1;
		return 0;
	} else return -1;
}

int remove_element(list * l, unsigned int index){
	if(l->size < 1) return -1;
	if(l->size == 1) {
		list_node *node = l->start;
		init(l);
		node->value = 0;
		node->next = NULL;
		free(node);
		return 0;
	}
	if(index >= l->size-1) return -1;
	list_node *node = l->start;
	if(index == 0) {
		l->start = node->next;
		l->size-=1;
		node->value = 0;
		node->next = NULL;
		free(node);
		return 0;
	}
	for(int i = 0; i < index-1; i++) node = node->next;
	list_node *deleting = NULL;
	if(index == l->size-1) {
		deleting = l->end;
		l->end = node;
	} else { deleting = node->next; }
	deleting->value = 0;
	node->next = deleting->next;
	deleting->next = NULL;
	free(deleting);
	l->size-=1;
	return 0;
}

int insert(list * l, unsigned int index, int data){
	if(index >= l->size) return -1;
	if(index == l->size-1) return append(l, data);
	list_node *node = l->start;
	for(int i = 0; i < index; i++) node = node->next;
	struct list_node *new = malloc(sizeof(struct list_node));
	if(new) {
		new->next = node->next;
		new->value = data;
		node->next = new;
		l->size+=1;
		return 0;
	} else return -1;
}

void print_list(list * l){
	if(l->size == 0) {
		printf("empty list\n");
		return;
	}
	list_node *node = l->start;
	for(int i = 0; i < l->size; i++) {
		printf("%d ",node->value);
		node = node->next;
	}
	printf("\n");
}

void init(list * l){
	l->start = NULL;
	l->end = NULL;
	l->size = 0;
}

void destroy_helper(list_node *node) {
	if(node->next) {
		destroy_helper(node->next);
	}
	node->value = 0;
	free(node);
}

void destroy(list *l){
	if(l->size != 0) destroy_helper(l->start);
	l->start = NULL;
	l->end = NULL;
	l->size = 0;
}
