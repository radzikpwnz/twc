#include "twc_design.h"

#include "clipboard.h"


/* Clipboard contents */
static DLIST_PTWC_OBJECT clipboard_content = {NULL, NULL, sizeof(TWC_OBJECT *)};


/**
 * Get clipboard contents.
 */
DLIST_PTWC_OBJECT *GetClipboard()
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
int AddObjectToClipboard( TWC_OBJECT *obj) /* object */
{
    ClearClipboard();
    DListAdd( &clipboard_content, (void *)-1, &obj);
    return 1;
}

/**
 * Place object list to clipboard.
 */
int AddObjectListToClipboard( DLIST_PTWC_OBJECT *obj_list) /* object list */
{
    ClearClipboard();
    DListClone( obj_list, NULL, (void *)-1, &clipboard_content);
    return 1;
}

/**
 * Paste object from clipboard.
 */
int PasteObjectsFromClipboard( TWC_OBJECT *parent) /* parent for objects copies */
{
    DLIST_NODE_PTWC_OBJECT *node;

    node = clipboard_content.first;
    while ( node != NULL ) {
        CreateObjectWindow(CopyObject(node->elem, parent), TWC_TRUE);
        node = node->next;
    }
    return 1;
}
