#include <windows.h>

#include "common.h"

#include "clipboard.h"


static DLIST_PRT_OBJECT selected_objects = {NULL, NULL, sizeof(RT_OBJECT *)};

static TWC_BOOL is_selection_active;
static x_start, y_start;
static RECT frame_rect;
static RT_OBJECT *parent_wnd;
static DLIST_NODE_PRT_OBJECT *prev_sel_end;


RECT *GetSelectionFrameRect()
{
    TWC_CHECKIT( is_selection_active );

    return &frame_rect;
}

DLIST_PRT_OBJECT *GetSelectedObjects()
{
    return &selected_objects;
}

int ClearSelection()
{
    OBJ_LIST_ITERATE_BEGIN( &selected_objects);
        node->elem->selected = 0;
        RedrawWindow(node->elem->hwnd, NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE | RDW_FRAME);
    OBJ_LIST_ITERATE_END();
    DListFree( &selected_objects);
    return 1;
}

int AddObjectToSelection( RT_OBJECT *obj)
{
    TWC_CHECKIT( !(obj->selected));

    obj->selected = 1;
    DListAdd( &selected_objects, (void *)-1, &obj);
    return 1;
}

int DeleteObjectFromSelection( RT_OBJECT *obj)
{
    TWC_CHECKIT( obj->selected);

    obj->selected = 0;
    DListRemove( &selected_objects, FindObjectInList( obj, &selected_objects));
    return 1;
}

TWC_BOOL IsSelectionActive()
{
    return is_selection_active;
}

int StartSelection( RT_OBJECT *parent, int x, int y, TWC_BOOL new)
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

int ProcessSelectionFrameChange( int new_x, int new_y)
{
    RECT update_rect, node_rect, res_rect;
    RT_OBJECT *obj;

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
        obj = node->elem;
        if ( !obj->selected ) {
            node_rect.left = obj->x;
            node_rect.top = obj->y;
            node_rect.right = node_rect.left + obj->width;
            node_rect.bottom = node_rect.top + obj->height;
            if ( IntersectRect( &res_rect, &node_rect, &frame_rect) ) {
                AddObjectToSelection( obj);
                RedrawWindow(node->elem->hwnd, NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE | RDW_FRAME);
                RedrawWindow(node->elem->static_hwnd, NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE | RDW_FRAME);
            }
        }
    OBJ_LIST_ITERATE_END();

    OBJ_LIST_ITERATE_BEGIN_FROM_NODE( ( prev_sel_end != NULL ) ? prev_sel_end->next : selected_objects.first);
        obj = node->elem;
        node_rect.left = obj->x;
        node_rect.top = obj->y;
        node_rect.right = node_rect.left + obj->width;
        node_rect.bottom = node_rect.top + obj->height;
        next = node->next;
        if ( !IntersectRect(&res_rect, &node_rect, &frame_rect) ) {
            obj->selected = 0;
            RedrawWindow(node->elem->hwnd, NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE | RDW_FRAME);
            RedrawWindow(node->elem->static_hwnd, NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE | RDW_FRAME);
            DListRemove( &selected_objects, node);
        }
    OBJ_LIST_ITERATE_END();

    //SendMessage(hwnd, WM_PAINT, 0, 0);
    UpdateWindow( parent_wnd->hwnd);

    return 1;
}

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