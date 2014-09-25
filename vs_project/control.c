#include <windows.h>

#include "common.h"

#include "toolbox.h"
#include "proplst.h"
#include "selection.h"
#include "control_act.h"

#include "control.h"


RT_OBJECT *CreateControlFromToolbox( RT_WINDOW *cur_wnd, int x, int y)
{
    RT_OBJECT *obj;

    if ( cur_wnd == NULL ) return NULL;

    obj = NewObject( GetCurrentToolboxControlId());

    PrepareObject( obj);
    SetNewObjectDefaultValues( obj);

    SetObjectPropertyInt( obj, COMMON_X, x, TWC_TRUE, TWC_FALSE);
    SetObjectPropertyInt( obj, COMMON_Y, y, TWC_TRUE, TWC_FALSE);
    SetObjectPropertyInt( obj, COMMON_WIDTH, CONTROL_MIN_WIDTH, TWC_TRUE, TWC_FALSE);
    SetObjectPropertyInt( obj, COMMON_HEIGHT, CONTROL_MIN_HEIGHT, TWC_TRUE, TWC_FALSE);

    obj->parent = cur_wnd;

    if ( CreateObjectWindow( obj, TWC_FALSE) == 0 )
    {
        FreeObject( obj);
        return NULL;
    }

    obj->lstnode_ptr = DListAdd( &cur_wnd->child_list, (void *)-1, &obj);
    GenerateObjectName( obj);

    return obj;
}

LRESULT CALLBACK ControlWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    RT_OBJECT *obj;
    RECT rect;

    obj = GetProp( hwnd, T("OBJECT_INFO"));

    switch ( msg ) {
        case WM_NCDESTROY:
            RemoveProp( hwnd, T("OBJECT_INFO"));
            DestroyWindow( obj->hwnd);
            break;
        case WM_MOVE:
			GetWindowRect( hwnd, &rect);
			MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (POINT *)&rect, 1);
			SetObjectPropertyInt(obj, COMMON_X, rect.left, TWC_TRUE, TWC_FALSE);
            SetObjectPropertyInt(obj, COMMON_Y, rect.top, TWC_TRUE, TWC_FALSE);

            SetWindowPos( obj->static_hwnd, NULL, rect.left, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			break;
		case WM_SIZE:
			GetWindowRect( hwnd, &rect);
			SetObjectPropertyInt(obj, COMMON_WIDTH, rect.right - rect.left, TWC_TRUE, TWC_FALSE);
			SetObjectPropertyInt(obj, COMMON_HEIGHT, rect.bottom - rect.top, TWC_TRUE, TWC_FALSE);

            SetWindowPos( obj->static_hwnd, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);
			break;
        case WM_NCHITTEST:
            return HTTRANSPARENT;
    }

    return CallWindowProc( obj->orig_wndproc, hwnd, msg, wParam, lParam);
}