#include <windows.h>

#include "common.h"


DLIST_NODE_PRT_OBJECT *FindObjectInList( RT_OBJECT *obj, DLIST_PRT_OBJECT *list)
{
    DLIST_NODE_PRT_OBJECT *node;

    node = list->first;
    while ( node != NULL ) {
        if ( node->elem == obj ) {
            return node;
        }
        node = node->next;
    }
    return NULL;
}

int PerformActionOnObjectsInList( DLIST_PRT_OBJECT *list, int (*func)(RT_OBJECT *))
{
    DLIST_NODE_PRT_OBJECT *node;

    node = list->first;
    while ( node != NULL ) {
        if ( func( node->elem) == 0 ) {
            return 0;
        }
        node = node->next;
    }

    return 1;
}

int GetObjectRect( RT_OBJECT *obj, RECT *rect)
{
    rect->left = obj->x;
    rect->right = obj->x + obj->width;
    rect->top = obj->y;
    rect->bottom = obj->y + obj->height;

    return 1;
}

int RoundByGrid( int x)
{
    return x - abs(x) % grid_size;
}