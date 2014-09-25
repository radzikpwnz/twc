#include <windows.h>
#include "mylist.h"

#define NEXT(x) (*((void **)x))
#define PREV(x) (*((void **)x + 1))
#define ELEM(x) ((void **)x + 2)

#define FIRST(x) (((DLIST_ABSTRACT *)x)->first)
#define LAST(x) (((DLIST_ABSTRACT *)x)->last)
#define ELEMSIZE(x) (((DLIST_ABSTRACT *)x)->elem_size)

void myfree(void **p);

//#define free(x) myfree(&(x))


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
	FIRST(list) = LAST(list) = NULL;
	return;
}
