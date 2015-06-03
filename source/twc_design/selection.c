#include <windows.h>

#include "twc_design.h"

#include "clipboard.h"

/* Selected objects list */
static DLIST_PTWC_OBJECT selected_objects = {NULL, NULL, sizeof(TWC_OBJECT *)};

/* Flag indicates that selection is active */
static TWC_BOOL is_selection_active;

/* Start point of selection */
static int x_start, y_start;

/* Selection frame rect */
static RECT frame_rect;

/* Parent window object */
static TWC_OBJECT *parent_wnd;

/* Previous selection end in selected objects list */
static DLIST_NODE_PTWC_OBJECT *prev_sel_end;


/**
 * Get selection frame rect.
 */
RECT *GetSelectionFrameRect()
{
    TWC_CHECKIT( is_selection_active );

    return &frame_rect;
}

/**
 * Get selected objects.
 */
DLIST_PTWC_OBJECT *GetSelectedObjects()
{
    return &selected_objects;
}

/**
 * Clear selection.
 */
int ClearSelection()
{
    OBJ_LIST_ITERATE_BEGIN( &selected_objects);
        OBJ_CLIENT_DATA( NODE()->elem)->selected = 0;
        RedrawWindow( NODE()->elem->hwnd, NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE | RDW_FRAME);
    OBJ_LIST_ITERATE_END();

    DListFree( &selected_objects);
    return 1;
}

/**
 * Add object to selection
 */
int AddObjectToSelection( TWC_OBJECT *obj) /* object */
{
    TWC_CHECKIT( !(OBJ_CLIENT_DATA( obj)->selected));

    OBJ_CLIENT_DATA( obj)->selected = 1;
    DListAdd( &selected_objects, (void *)-1, &obj);
    return 1;
}

/**
 * Delete object from selection.
 */
int DeleteObjectFromSelection( TWC_OBJECT *obj) /* object */
{
    TWC_CHECKIT( OBJ_CLIENT_DATA( obj)->selected);

    OBJ_CLIENT_DATA( obj)->selected = 0;
    DListRemove( &selected_objects, FindObjectInList( obj, &selected_objects));
    return 1;
}

/**
 * Returns selection active flag.
 */
TWC_BOOL IsSelectionActive()
{
    return is_selection_active;
}

/**
 * Start selection.
 */
int StartSelection( TWC_OBJECT *parent, /* parent object */
                    int x, int y,      /* coordinates of start point*/
                    TWC_BOOL new)      /* new selection? */
{
    TWC_CHECKIT( !is_selection_active );
    TWC_CHECKIT( GetCapture() == NULL );

    if ( new ) {
        ClearSelection();
        prev_sel_end = NULL;
    }
    x_start = frame_rect.left = frame_rect.right = x;
    y_start = frame_rect.top = frame_rect.bottom = y;
    parent_wnd = parent;
    SetCapture( parent->hwnd);

    is_selection_active = TWC_TRUE;
    return 1;
}

/**
 * Process selection frame change.
 */
int ProcessSelectionFrameChange( int new_x, int new_y) /* new coordinates */
{
    RECT update_rect, node_rect, res_rect;
    TWC_OBJECT *obj;

    TWC_CHECKIT( is_selection_active );

    update_rect.left = MIN( frame_rect.left, new_x - 30);
    update_rect.right = MAX( frame_rect.right, new_x + 30);
    update_rect.top = MIN( frame_rect.top, new_y - 30);
    update_rect.bottom = MAX( frame_rect.bottom, new_y + 30);

    //RedrawWindow(hwnd, &rect, NULL, RDW_NOCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW);
    InvalidateRect( parent_wnd->hwnd, &update_rect, 0);

    if ( new_x < x_start) {
        frame_rect.left = new_x;
    } else {
        frame_rect.right = new_x;
    }
    if ( new_y < y_start) {
        frame_rect.top = new_y;
    } else {
        frame_rect.bottom = new_y;
    }

    OBJ_LIST_ITERATE_BEGIN( &parent_wnd->child_list);
        obj = NODE()->elem;
        if ( !OBJ_CLIENT_DATA( obj)->selected ) {
            node_rect.left = obj->x;
            node_rect.top = obj->y;
            node_rect.right = node_rect.left + obj->width;
            node_rect.bottom = node_rect.top + obj->height;
            if ( IntersectRect( &res_rect, &node_rect, &frame_rect) ) {
                AddObjectToSelection( obj);
                RedrawWindow( obj->hwnd, NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE | RDW_FRAME);
                RedrawWindow( OBJ_CLIENT_DATA( obj)->static_hwnd, NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE | RDW_FRAME);
            }
        }
    OBJ_LIST_ITERATE_END();

    OBJ_LIST_ITERATE_BEGIN_FROM_NODE( ( prev_sel_end != NULL ) ? prev_sel_end->next : selected_objects.first);
        obj = NODE()->elem;
        node_rect.left = obj->x;
        node_rect.top = obj->y;
        node_rect.right = node_rect.left + obj->width;
        node_rect.bottom = node_rect.top + obj->height;
        if ( !IntersectRect( &res_rect, &node_rect, &frame_rect) ) {
            OBJ_CLIENT_DATA( obj)->selected = 0;
            RedrawWindow( obj->hwnd, NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE | RDW_FRAME);
            RedrawWindow( OBJ_CLIENT_DATA( obj)->static_hwnd, NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE | RDW_FRAME);
            DListRemove( &selected_objects, NODE());
        }
    OBJ_LIST_ITERATE_END();

    //SendMessage(hwnd, WM_PAINT, 0, 0);
    UpdateWindow( parent_wnd->hwnd);

    return 1;
}

/**
 * Stop selection.
 */
void StopSelection()
{
    TWC_CHECKIT( is_selection_active );
    TWC_CHECKIT( GetCapture() == parent_wnd->hwnd );

    prev_sel_end = selected_objects.last;

    is_selection_active = TWC_FALSE;

    ReleaseCapture();
    InvalidateRect( parent_wnd->hwnd, &frame_rect, 0);
    UpdateWindow( parent_wnd->hwnd);

    return;
}
