#include <windows.h>

#include "twc_design.h"

#include "toolbox.h"
#include "properties.h"

#include "control.h"

/**
 * Create control from toolbox.
 */
TWC_OBJECT *CreateControlFromToolbox( TWC_OBJECT *cur_wnd, /* parent window object */
                                     int x, int y)       /* position */
{
    TWC_OBJECT *obj;

    TWC_CHECKIT ( cur_wnd != NULL );

    /* Create object */
    obj = twc_NewObject( GetCurrentToolboxControlId());

    /* Prepare object and load defaults */
    twc_PrepareObject( obj);
    SetNewObjectDefaultValues( obj);

    /* Set object position and size */
    SetObjectPropertyInt( obj, COMMON_X, x, TWC_TRUE, TWC_FALSE);
    SetObjectPropertyInt( obj, COMMON_Y, y, TWC_TRUE, TWC_FALSE);
    SetObjectPropertyInt( obj, COMMON_WIDTH, CONTROL_MIN_WIDTH, TWC_TRUE, TWC_FALSE);
    SetObjectPropertyInt( obj, COMMON_HEIGHT, CONTROL_MIN_HEIGHT, TWC_TRUE, TWC_FALSE);

    obj->parent = cur_wnd;

    /* Create object window */
    if ( CreateObjectWindow( obj, TWC_FALSE) == 0 )
    {
        FreeObject( obj);
        return NULL;
    }

    /* Add object to parent's child list */
    obj->lstnode_ptr = DListAdd( &cur_wnd->child_list, (void *)-1, &obj);
    GenerateObjectName( obj);

    return obj;
}

/**
 * Control window procedure.
 */
LRESULT CALLBACK ControlWndProc( TWC_OBJECT *obj, UINT msg, WPARAM wParam, LPARAM lParam)
{
    RECT rect;
    HWND hwnd = obj->hwnd;

    switch ( msg ) {
        case WM_NCDESTROY:
            /* Destroy static window */
            DestroyWindow( OBJ_CLIENT_DATA( obj)->static_hwnd);
            break;
        case WM_MOVE:
            /* Write new values to properties */
			GetWindowRect( hwnd, &rect);
			MapWindowPoints( HWND_DESKTOP, GetParent(hwnd), (POINT *)&rect, 1);
			SetObjectPropertyInt( obj, COMMON_X, rect.left, TWC_TRUE, TWC_FALSE);
            SetObjectPropertyInt( obj, COMMON_Y, rect.top, TWC_TRUE, TWC_FALSE);

            /* Move static window */
            SetWindowPos( OBJ_CLIENT_DATA( obj)->static_hwnd, NULL, rect.left, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			break;
		case WM_SIZE:
            /* Write new values to properties */
			GetWindowRect( hwnd, &rect);
			SetObjectPropertyInt( obj, COMMON_WIDTH, rect.right - rect.left, TWC_TRUE, TWC_FALSE);
			SetObjectPropertyInt( obj, COMMON_HEIGHT, rect.bottom - rect.top, TWC_TRUE, TWC_FALSE);

            /* Size static window */
            SetWindowPos( OBJ_CLIENT_DATA( obj)->static_hwnd, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);
			break;
        case WM_NCHITTEST:
            /* Do not react on mouse */
            return HTTRANSPARENT;
    }

    return CallWindowProc( obj->orig_wndproc, hwnd, msg, wParam, lParam);
}
