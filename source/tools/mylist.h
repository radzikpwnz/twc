#ifndef MYLIST_H
#define MYLIST_H

/*
Universal C list, fast and easy to use

Author: R@DZIK

Contents:
	mylist.h - header file
	mylist.c - source

Usage:
	
	DLIST_CUSTOM(<type>); //declare list type with specific item type
	DLIST_<type> var;
	You must initialize members of DLIST struct manually to {NULL, NULL, sizeof(type), 0}
*/

typedef struct tagDLIST_ABSTRACT{
	void *first;
	void *last;
	UINT elem_size;
	UINT count;
} DLIST_ABSTRACT;

#define DLIST_NODE(type) \
	struct tagDLIST_NODE_##type { \
		struct tagDLIST_NODE_##type *next; \
		struct tagDLIST_NODE_##type *prev; \
		type elem; \
	}

#define DLIST(type) \
	struct { \
		DLIST_NODE(type) *first; \
		DLIST_NODE(type) *last; \
		UINT elem_size; \
		UINT count; \
	}

#define DLIST_CUSTOM(type) \
	typedef DLIST_NODE(type) DLIST_NODE_##type; \
	typedef struct tagDLIST_##type { \
		DLIST_NODE_##type *first; \
		DLIST_NODE_##type *last; \
		UINT elem_size; \
		UINT count; \
	} DLIST_##type;



#define DLIST_NODE_FD(type) \
	struct tagDLIST_NODE_##type

#define DLIST_NODE_RD(type) \
	struct tagDLIST_NODE_##type { \
		struct tagDLIST_NODE_##type *next; \
		struct tagDLIST_NODE_##type *prev; \
		type elem; \
	}

#define DLIST_CUSTOM_FD(type) \
	typedef DLIST_NODE_FD(type) DLIST_NODE_##type; \
	typedef struct tagDLIST_##type DLIST_##type;

#define DLIST_CUSTOM_RD(type) \
	DLIST_NODE_RD(type);\
	struct tagDLIST_##type{ \
		DLIST_NODE_##type *first; \
		DLIST_NODE_##type *last; \
		UINT elem_size; \
		UINT count; \
	};


void *DListAdd(void *list, void *ins_after, void *elem);
/*
Add item to list.
list - pointer to DLIST struct
ins_after - pointer to elem, after witch item will be added
elem - pointer to item to add
*/

void *DListAddExist(void *list, void *ins_after, void *elem);

void DListGet(void *list, void *pos, void *buf, int remove);
/*
Get item from list.
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
/*
Free list.
list - pointer to DLIST struct
*/

void DListMerge(void *list1, void *list2);
/*
Merge two lists.
*/

void DListInit(void *list, UINT elem_size);

void DListInsertList(void *list, void *ins_after, void *list_to_insert);

void *DListElemToNode(void *elem);

void *DListAlloc(void *list);

void DListClone(void *src, void *dst);

#endif