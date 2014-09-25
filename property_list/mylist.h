#ifndef MYLIST_H
#define MYLIST_H

/*
Universal C queue, fast and easy to use

Author: R@DZIK

Contents:
	myqueue.h - header file
	myqueue.c - source

Usage:
	QUEUE(<type>) var;
	You must initialize members of QUEUE struct manually to {NULL, NULL, sizeof(type)}
*/

typedef struct {
	void *first;
	void *last;
	int elem_size;
} DLIST_ABSTRACT;

#define DLIST_NODE(type) \
	struct { \
		void *next; \
		void *prev; \
		type elem; \
	}

#define DLIST(type) \
	struct { \
		DLIST_NODE(type) *first; \
		DLIST_NODE(type) *last; \
		int elem_size; \
	}

void *DListAdd(void *list, void *ins_after, void *elem);
/*
Add item to list.
list - pointer to DLIST struct
ins_after - pointer to elem, after witch item will be added
elem - pointer to item to add
*/

void DListGet(void *list, void *pos, void *buf, int remove);
/*
Get item from queue.
list - pointer to DLIST struct
pos - pointer to item to get
buf - pointer to buffer
remove - 1 to remove item from list, 0 - don't remove
*/

void DListRemove(void *list, void *pos);
/*
Remove item from list.
list - pointer to DLIST struct
pos - pointer to item to remove
*/

void DListFree(void *list);



#endif