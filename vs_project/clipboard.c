#include <windows.h>

#include "common.h"


static DLIST_PRT_OBJECT clipboard_content = {NULL, NULL, sizeof(RT_OBJECT *)};


DLIST_PRT_OBJECT *GetCurrentClipboard()
{
    return &clipboard_content;
}

int ClearClipboard()
{
    DListFree( &clipboard_content);
    return 1;
}

int PlaceObjectToClipboard( RT_OBJECT *obj)
{
    ClearClipboard();
    DListAdd( &clipboard_content, (void *)-1, &obj);
    return 1;
}

int PlaceObjectListToClipboard( DLIST_PRT_OBJECT *obj_list)
{
    ClearClipboard();
    DListClone( obj_list, &clipboard_content);
    return 1;
}

int PasteObjectsFromClipboard( RT_OBJECT *parent)
{
    DLIST_NODE_PRT_OBJECT *node;

    node = clipboard_content.first;
    while ( node != NULL ) {
        CreateObjectWindow(CopyObject(node->elem, parent), TWC_TRUE);
        node = node->next;
    }
    return 1;
}