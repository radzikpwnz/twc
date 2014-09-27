#include "twc_design.h"

#include "clipboard.h"


/* Clipboard contents */
static DLIST_PRT_OBJECT clipboard_content = {NULL, NULL, sizeof(RT_OBJECT *)};


/**
 * Get clipboard contents.
 */
DLIST_PRT_OBJECT *GetClipboard()
{
    return &clipboard_content;
}

/**
 * Clear clipboard.
 */
int ClearClipboard()
{
    DListFree( &clipboard_content);
    return 1;
}

/**
 * Place object to clipboard.
 */
int AddObjectToClipboard( RT_OBJECT *obj) /* object */
{
    ClearClipboard();
    DListAdd( &clipboard_content, (void *)-1, &obj);
    return 1;
}

/**
 * Place object list to clipboard.
 */
int AddObjectListToClipboard( DLIST_PRT_OBJECT *obj_list) /* object list */
{
    ClearClipboard();
    DListClone( obj_list, &clipboard_content);
    return 1;
}

/**
 * Paste object from clipboard.
 */
int PasteObjectsFromClipboard( RT_OBJECT *parent) /* parent for objects copies */
{
    DLIST_NODE_PRT_OBJECT *node;

    node = clipboard_content.first;
    while ( node != NULL ) {
        CreateObjectWindow(CopyObject(node->elem, parent), TWC_TRUE);
        node = node->next;
    }
    return 1;
}
