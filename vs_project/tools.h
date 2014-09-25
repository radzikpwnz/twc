#ifndef TOOLS_H
#define TOOLS_H

DLIST_NODE_PRT_OBJECT *FindObjectInList( RT_OBJECT *obj, DLIST_PRT_OBJECT *list);
int PerformActionOnObjectsInList( DLIST_PRT_OBJECT *list, int (*func)(RT_OBJECT *));
int GetObjectRect( RT_OBJECT *obj, RECT *rect);
int RoundByGrid( int x);

#endif