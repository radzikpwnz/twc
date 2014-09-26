#include <windows.h>

#include "twc_design.h"

#include "clipboard.h"
#include "selection.h"

#include "control_act.h"

#define MAX_CTRL_MOVE_SPEED 20

/* Control move speed */
static int ctrl_move_speed = 1;

/**
 * Delete selected objects.
 */
int DeleteSelectedObjects()
{
    /* If current object among selected, set current object to parent */
    if ( current_object != NULL && FindObjectInList( current_object, GetSelectedObjects()) != NULL ) {
        SetCurrentObject( GetSelectedObjects()->first->elem->parent);
    }

    OBJ_LIST_ITERATE_BEGIN( GetSelectedObjects());
        DestroyObjectWindow( node->elem, TWC_TRUE);
    OBJ_LIST_ITERATE_END();

    DListFree( GetSelectedObjects());

    return 1;
}

/**
 * Place selected objects to clipboard.
 */
int CopySelectedObjects()
{
    PlaceObjectListToClipboard( GetSelectedObjects());
    return 1;
}

/**
 * Move selected objects.
 *
 * (new_x, new_y) = (x + dx, y + dy)
 */
int MoveSelectedObjects( int dx, int dy)
{
    OBJ_LIST_ITERATE_BEGIN( GetSelectedObjects());
        SetWindowPos( node->elem->hwnd, NULL, node->elem->x + dx, node->elem->y + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    OBJ_LIST_ITERATE_END();

    return 1;
}

/**
 * Move selected objects using keyboard.
 */
int MoveSelectedObjectsKey( UINT dir) /* key pressed */
{
    switch ( dir ) {
        case VK_DOWN:
            MoveSelectedObjects( 0, ctrl_move_speed);
            break;
        case VK_UP:
            MoveSelectedObjects( 0, -ctrl_move_speed);
            break;
        case VK_RIGHT:
            MoveSelectedObjects( ctrl_move_speed, 0);
            break;
        case VK_LEFT:
            MoveSelectedObjects( -ctrl_move_speed, 0);
            break;
    }

    if ( ctrl_move_speed < MAX_CTRL_MOVE_SPEED ) {
        ctrl_move_speed++;
    }

    return 1;
}

/**
 * Stop moving objects using keyboard.
 */
int StopMovingObjectsKey()
{
    ctrl_move_speed = 1;

    return 1;
}
