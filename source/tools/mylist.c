#include <windows.h>
#include "mylist.h"

#define NEXT(x) (*((void **)x))
#define PREV(x) (*((void **)x + 1))
#define ELEM(x) ((void **)x + 2)

#define FIRST(x) (((DLIST_ABSTRACT *)x)->first)
#define LAST(x) (((DLIST_ABSTRACT *)x)->last)
#define ELEMSIZE(x) (((DLIST_ABSTRACT *)x)->elem_size)
#define COUNT(x) (((DLIST_ABSTRACT *)x)->count)

void *DListAdd(void *list, void *ins_after, void *elem)
{
	void *pnew;

	pnew = malloc(ELEMSIZE(list) + 2 * sizeof(void *));
	if (FIRST(list) == NULL) {
		NEXT(pnew) = NULL;
		PREV(pnew) = NULL;
		FIRST(list) = pnew;
		LAST(list) = pnew;
	} else if (ins_after == NULL) {
		NEXT(pnew) = FIRST(list);
		PREV(pnew) = NULL;
		PREV(FIRST(list))= pnew;
		FIRST(list) = pnew;
	} else if (ins_after == (void *)-1 || ins_after == LAST(list)) {
		NEXT(pnew) = NULL;
		PREV(pnew) = LAST(list);
		NEXT(LAST(list)) = pnew;
		LAST(list) = pnew;
	} else {
		NEXT(pnew) = NEXT(ins_after);
		PREV(pnew) = ins_after;
		NEXT(ins_after) = pnew;
		PREV(NEXT(pnew)) = pnew;
	}

	memcpy(ELEM(pnew), elem, ELEMSIZE(list));
	COUNT(list)++;
	return pnew;
}

void *DListAddExist(void *list, void *ins_after, void *elem)
{
	void *pnew;

	pnew = elem;
	if (FIRST(list) == NULL) {
		NEXT(pnew) = NULL;
		PREV(pnew) = NULL;
		FIRST(list) = pnew;
		LAST(list) = pnew;
	} else if (ins_after == NULL) {
		NEXT(pnew) = FIRST(list);
		PREV(pnew) = NULL;
		PREV(FIRST(list))= pnew;
		FIRST(list) = pnew;
	} else if (ins_after == (void *)-1 || ins_after == LAST(list)) {
		NEXT(pnew) = NULL;
		PREV(pnew) = LAST(list);
		NEXT(LAST(list)) = pnew;
		LAST(list) = pnew;
	} else {
		NEXT(pnew) = NEXT(ins_after);
		PREV(pnew) = ins_after;
		NEXT(ins_after) = pnew;
		PREV(NEXT(pnew)) = pnew;
	}

	//memcpy(ELEM(pnew), elem, ELEMSIZE(list));
	COUNT(list)++;
	return pnew;
}

void *DListAddCustomSize(void *list, void *ins_after, void *elem, UINT size)
{
	void *pnew;

	pnew = malloc(size + 2 * sizeof(void *));
	if (FIRST(list) == NULL) {
		NEXT(pnew) = NULL;
		PREV(pnew) = NULL;
		FIRST(list) = pnew;
		LAST(list) = pnew;
	} else if (ins_after == NULL) {
		NEXT(pnew) = FIRST(list);
		PREV(pnew) = NULL;
		PREV(FIRST(list))= pnew;
		FIRST(list) = pnew;
	} else if (ins_after == (void *)-1 || ins_after == LAST(list)) {
		NEXT(pnew) = NULL;
		PREV(pnew) = LAST(list);
		NEXT(LAST(list)) = pnew;
		LAST(list) = pnew;
	} else {
		NEXT(pnew) = NEXT(ins_after);
		PREV(pnew) = ins_after;
		NEXT(ins_after) = pnew;
		PREV(NEXT(pnew)) = pnew;
	}

	memcpy(ELEM(pnew), elem, size);
	COUNT(list)++;
	return pnew;
}

void DListGet(void *list, void *pos, void *buf, int remove)
{
	if (pos == NULL) return;
	memcpy(buf, ELEM(pos), ELEMSIZE(list));
	if (remove) {
		DListRemove(list, pos);
	}
	return;
}

void DListRemove(void *list, void *pos)
{
	void *pnext, *pprev;

	if (pos == NULL) return;
	pprev = PREV(pos);
	pnext = NEXT(pos);
	if (pprev) {
		NEXT(pprev) = pnext;
	} else {
		FIRST(list) = pnext;
	}
	if (pnext) {
		PREV(pnext) = pprev;
	} else {
		LAST(list) = pprev;
	}
	free(pos);
	COUNT(list)--;
	return;
}

void DListFree(void *list)
{
	void *ptr, *next;

	ptr = FIRST(list);
	while (ptr != NULL) {
		next = NEXT(ptr);
		free(ptr);
		ptr = next;
	}
	COUNT(list) = 0;
	FIRST(list) = LAST(list) = NULL;
	return;
}

void DListMerge(void *list1, void *list2)
{
	if (FIRST(list1) == NULL) {
		FIRST(list1) = FIRST(list2);
		LAST(list1) = LAST(list2);
		COUNT(list1) = COUNT(list2);
	} else {
		NEXT(LAST(list1)) = FIRST(list2);
		if (FIRST(list2)) {
			PREV(FIRST(list2)) = LAST(list1);
		}
		LAST(list1) = LAST(list2);
		COUNT(list1) += COUNT(list2);
	}
	return;
}

void DListInit(void *list, UINT elem_size)
{
	FIRST(list) = LAST(list) = NULL;
	COUNT(list) = 0;
	ELEMSIZE(list) = elem_size;
	return;
}

void DListInsertList(void *list, void *ins_after, void *list_to_insert)
{
	if (FIRST(list_to_insert) == NULL) return;

	if (FIRST(list) == NULL) {
		FIRST(list) = FIRST(list_to_insert);
		LAST(list) = LAST(list_to_insert);
	} else if (ins_after == NULL) {
		NEXT(LAST(list_to_insert)) = FIRST(list);
		PREV(FIRST(list))= LAST(list_to_insert);
		FIRST(list) = FIRST(list_to_insert);
	} else if (ins_after == (void *)-1 || ins_after == LAST(list)) {
		NEXT(LAST(list_to_insert)) = FIRST(list);
		PREV(FIRST(list)) = LAST(list_to_insert);
		LAST(list) = LAST(list_to_insert);
	} else {
		NEXT(LAST(list_to_insert)) = NEXT(ins_after);
		PREV(FIRST(list_to_insert)) = ins_after;

		PREV(NEXT(ins_after)) = LAST(list_to_insert);
		NEXT(ins_after) = FIRST(list_to_insert);
	}

	COUNT(list) += COUNT(list_to_insert);
	return;
}

void *DListElemToNode(void *elem)
{
	return (BYTE *)elem - 2 * sizeof(void *);
}

void *DListAlloc(void *list)
{
	return malloc(ELEMSIZE(list) + 2 * sizeof(void *));
}

void DListExchange(void *pos1, void *pos2)
{
	if (PREV(pos1)) NEXT(PREV(pos1)) = pos2;
	if (NEXT(pos1)) PREV(NEXT(pos1)) = pos2;

	if (PREV(pos2)) NEXT(PREV(pos2)) = pos1;
	if (NEXT(pos2)) PREV(NEXT(pos2)) = pos1;

	return;
}

void DListMove(void *list, void *node, void *ins_after)
{
	if (node == ins_after) return;

	if (PREV(node)) NEXT(PREV(node)) = NEXT(node);
	if (NEXT(node)) PREV(NEXT(node)) = PREV(node);

	//if (PREV(ins_after)) NEXT(PREV(ins_after)) = node;
	//if (NEXT(ins_after)) PREV(NEXT(ins_after)) = node;

	if (ins_after == NULL) {
		NEXT(node) = FIRST(list);
		PREV(node) = NULL;
		PREV(FIRST(list))= node;
		FIRST(list) = node;
	} else if (ins_after == (void *)-1 || ins_after == LAST(list)) {
		NEXT(node) = NULL;
		PREV(node) = LAST(list);
		NEXT(LAST(list)) = node;
		LAST(list) = node;
	} else {
		NEXT(node) = NEXT(ins_after);
		PREV(node) = ins_after;
		NEXT(ins_after) = node;
		PREV(NEXT(node)) = node;
	}

	return;
}

void DListClone(void *src, void *first, void *last, void *dst)
{
	BYTE *start, *p, *prev;
	void *node, *last_node;
	int node_size;
	int count;

	node_size = ELEMSIZE(src) + 2 * sizeof(void *);
	prev = malloc(node_size);
	start = p = malloc(node_size);

	node = (first == NULL) ? FIRST(src) : first;
	last_node = (last == (void *)-1) ? NULL : NEXT(last);
	FIRST(dst) = p;
	count = 0;
	while (node != last_node) {
		memcpy(ELEM(p), ELEM(node), ELEMSIZE(src));
		PREV(p) = prev;
		NEXT(prev) = p;

		prev = p;
		p = malloc(node_size);
		node = NEXT(node);
		count++;
	}
	free(p);
	p = prev;
	NEXT(p) = NULL;
	free(PREV(start));
	PREV(start) = NULL;
	LAST(dst) = p;
	ELEMSIZE(dst) = ELEMSIZE(src);
	COUNT(dst) = count;
}