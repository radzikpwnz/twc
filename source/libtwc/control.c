#include "libtwc.h"

#include "object.h"
#include "properties.h"

#include "control.h"

TWC_OBJECT *twc_CreateControl( UINT ctrl_id,
                               TWC_OBJECT *parent,
                               int x, int y,
                               int width, int height)
{
    TWC_OBJECT *obj;

    /* Create object */
    obj = twc_NewObject( ctrl_id);

    /* Prepare object */
    twc_PrepareObject( obj);

    /* Set object position and size */
    twc_SetObjectPropertyInt( obj, COMMON_X, x, TWC_TRUE, TWC_FALSE);
    twc_SetObjectPropertyInt( obj, COMMON_Y, y, TWC_TRUE, TWC_FALSE);
    twc_SetObjectPropertyInt( obj, COMMON_WIDTH, width, TWC_TRUE, TWC_FALSE);
    twc_SetObjectPropertyInt( obj, COMMON_HEIGHT, height, TWC_TRUE, TWC_FALSE);

    obj->parent = parent;

    /* Create object window */
    if ( twc_CreateObjectWindow( obj, TWC_FALSE, HWND_TOP) == 0 ) {
        twc_FreeObject( obj);
        return NULL;
    }

    /* Add object to parent's child list */
    obj->lstnode_ptr = DListAdd( &parent->child_list, (void *)-1, &obj);

    return obj;
}

