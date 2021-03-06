#include <windows.h>

#include "twc_design.h"

#include "interface.h"
#include "proplst.h"
#include "mainwnd.h"
#include "toolbox.h"
#include "selection.h"
#include "control_act.h"
#include "clipboard.h"
#include "static.h"
#include "project.h"
#include "properties.h"

#include "window.h"


static int DeleteWindow( TWC_OBJECT *obj)
{
    if ( MessageBox( hMainWnd, T("Delete window?"), T("TWC Design"), MB_YESNO) == IDNO ) {
        return 0;
    }

    if ( obj == current_object ) {
        SetCurrentObject( NULL);
    }
    SendMessage( hPropList, PL_CLEAN, 0, 0);
    DestroyObjectWindow( obj, TWC_TRUE);
    return 1;
}

static int PaintWindow( HWND hwnd)
{
    RECT rect, *frame_rect;
    PAINTSTRUCT ps;
    HDC hDC;
    HBRUSH hOldBrush;

    /* Background */
    hDC = BeginPaint( hwnd, &ps);
    GetClientRect( hwnd, &rect);
    FillRect( hDC, &rect, (HBRUSH)(COLOR_BTNFACE + 1));

    /* Grid */
    /*hOldBrush = SelectObject(hDC, GetStockObject(NULL_BRUSH));
    for (i = 0; i < rect.right; i+= grid_size) {
        MoveToEx(hDC, i, 0, NULL);
        LineTo(hDC, i, rect.bottom);
    }
    for (i = 0; i < rect.bottom; i+= grid_size) {
        MoveToEx(hDC, 0, i, NULL);
        LineTo(hDC, rect.right, i);
    }
    SelectObject(hDC, hOldBrush);*/

    /* Selection Frame */
    if ( IsSelectionActive() ) {
        frame_rect = GetSelectionFrameRect();
        hOldBrush = SelectObject( hDC, GetStockObject(NULL_BRUSH));
        Rectangle( hDC, frame_rect->left, frame_rect->top, frame_rect->right, frame_rect->bottom);
        SelectObject( hDC, hOldBrush);
    }
    EndPaint(hwnd, &ps);

    return 1;
}

LRESULT CALLBACK ChildWndProc( TWC_OBJECT *obj, UINT msg, WPARAM wParam, LPARAM lParam)
{
    RECT rect;
    POINTS p;
    static int ctrl_move_speed = 1;
    TWC_BOOL is_ctrl_pressed;
    HWND hwnd = obj->hwnd;

    switch (msg) {
        case WM_CLOSE:
            if ( !DeleteWindow( obj) ) {
                return 0;
            }
            break;
        case WM_PAINT:
            PaintWindow( hwnd);
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_KEYDOWN:
            is_ctrl_pressed = (GetAsyncKeyState(VK_CONTROL) >> (sizeof(SHORT) - 1));
            if ( is_ctrl_pressed ) {
                switch ( wParam ) {
                    case 0x43:
                        /* Ctrl-C */
                        CopySelectedObjects();
                        break;
                    case 0x56:
                        /* Ctrl-V */
                        PasteObjectsFromClipboard( obj);
                        break;
                    case 0x53:
                        /* Ctrl-S */
                        actSaveProject();
                        break;
                }
            } else {
                switch ( wParam ) {
                    case VK_DELETE:
                        DeleteSelectedObjects();
                        break;
                    case VK_DOWN:
                    case VK_UP:
                    case VK_LEFT:
                    case VK_RIGHT:
                        MoveSelectedObjectsKey( wParam);
                        break;
                }
            }
            break;
            //return 0;
        case WM_KEYUP:
            switch ( wParam ) {
                case VK_DOWN:
                case VK_UP:
                case VK_LEFT:
                case VK_RIGHT:
                    StopMovingObjectsKey();
                    break;
	        }
	        break;
		case WM_NCLBUTTONDOWN:
			if ( IsToolboxControlSelected() == TWC_FALSE ) {
				SetCurrentObject( obj);
				ClearSelection();
			}
			break;
		case WM_LBUTTONDOWN:
            if ( IsToolboxControlSelected() == TWC_FALSE ) {
                SetFocus(hwnd); //kostyl
                is_ctrl_pressed = ( GetAsyncKeyState( VK_CONTROL) >> (sizeof(SHORT) - 1) );
                StartSelection( obj, LOWORD(lParam), HIWORD(lParam), !is_ctrl_pressed );
                if ( !is_ctrl_pressed ) {
                    SetCurrentObject( obj);
                }
            } else {
                obj = CreateControlFromToolbox( obj, RoundByGrid( LOWORD(lParam)), RoundByGrid( HIWORD(lParam)));
                if ( obj ) {
                    new_control = 1;
                    //StaticWndProc( obj->static_hwnd, WM_LBUTTONDOWN, wParam, MAKELPARAM(0,0));
                    //((WNDPROC)GetWindowLongPtr( obj->static_hwnd, GWLP_WNDPROC))( obj->static_hwnd, WM_LBUTTONDOWN, wParam, MAKELPARAM(0,0));
                    SendMessage( OBJ_CLIENT_DATA( obj)->static_hwnd, WM_LBUTTONDOWN, wParam, MAKELPARAM(0,0));
                }
            }
            return 0;
		case WM_LBUTTONDBLCLK:
			CreateCodeWindow( obj, NULL);
			return 0;
		case WM_MOUSEMOVE:
			if ( IsSelectionActive() ) {
				p = MAKEPOINTS(lParam);
                ProcessSelectionFrameChange( p.x, p.y);
			}
			return 0;
		case WM_LBUTTONUP:
			if ( IsSelectionActive() ) {
                StopSelection();
			}
			return 0;
        case WM_SIZE:
            if ( obj->flags & OBJ_FLAG_CLIENTSIZE ) {
			    GetClientRect( hwnd, &rect);
            } else {
                GetWindowRect( hwnd, &rect);
            }
			SetObjectPropertyInt( obj, COMMON_WIDTH, rect.right - rect.left, TWC_TRUE, TWC_FALSE);
			SetObjectPropertyInt( obj, COMMON_HEIGHT, rect.bottom - rect.top, TWC_TRUE, TWC_FALSE);
			
			InvalidateRect(hwnd, NULL, 0);
			UpdateWindow(hwnd);
			break;
    }
  
    return DefMDIChildProc(hwnd, msg, wParam, lParam);
}

HWND AddWindow()
{
    TWC_OBJECT *obj;
    RECT rect;

    obj = twc_NewObject( CTRL_ID_WINDOW);

    twc_PrepareObject( obj);
    SetNewObjectDefaultValues( obj);

    GetClientRect( MDIClient.hwnd, &rect);
    SetObjectPropertyInt( obj, COMMON_WIDTH, (rect.right - rect.left) / 2, TWC_TRUE, TWC_FALSE);
    SetObjectPropertyInt( obj, COMMON_HEIGHT, (rect.bottom - rect.top) / 2, TWC_TRUE, TWC_FALSE);

    obj->parent = cur_project->root_object;

    if ( CreateObjectWindow( obj, TWC_FALSE) == 0 )
    {
        FreeObject( obj);
        return NULL;
    }

	obj->lstnode_ptr = DListAdd( GetParentChildList( obj), (void *)-1, &obj);
	GenerateObjectName( obj);
	
    SetCurrentObject( obj);
    SetFocus( obj->hwnd);

	return obj->hwnd;
}

int InitControlView(TWC_OBJECT *obj, HWND hwnd)
{

	if (obj->id == CTRL_ID_PROGRESSBAR) {
		SendMessage(hwnd, PBM_SETPOS, 30, 0);
	} else if (obj->id == CTRL_ID_UPDOWN) {
		SetWindowPos(hwnd, NULL, 0, 0, obj->width, obj->height, SWP_NOMOVE | SWP_NOZORDER);
	} else if (obj->id == CTRL_ID_LISTBOX) {
		SendMessage(hwnd, LB_ADDSTRING, 0, (LPARAM)T("Item 1"));
		SendMessage(hwnd, LB_ADDSTRING, 0, (LPARAM)T("Item 2"));
		SendMessage(hwnd, LB_ADDSTRING, 0, (LPARAM)T("Item 3"));
	} else if (obj->id == CTRL_ID_COMBOBOX) {
		if (obj->style & CBS_UPPERCASE || obj->style & CBS_LOWERCASE) return 1;
		SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)T("Item 1"));
		SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)T("Item 2"));
		SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)T("Item 3"));
		SendMessage(hwnd, CB_SETCURSEL, 0, 0);
	}
	return 1;
}

static DWORD StyleFilterForPreview( TWC_OBJECT *obj)
{
    return obj->style;
}

static DWORD ExStyleFilterForPreview( TWC_OBJECT *obj)
{
    DWORD exstyle;

	exstyle = obj->exstyle;
	
    if ( IsObjectWindow( obj) )
    {
        exstyle = exstyle & ~(WS_EX_MDICHILD);
    }   

	return exstyle;
}

int PreviewWindow(TWC_OBJECT *obj, HWND hParent)
{
	RECT rect;
	HWND hwnd;
	int x, y;
	VALUE *val;

	if ( hParent == hMainWnd ) {
		GetWindowRect(hParent, &rect);
		x = (rect.right + rect.left) / 2 - obj->width / 2;
		y = (rect.bottom + rect.top) / 2 - obj->height / 2;
	} else {
		x = obj->x;
		y = obj->y;
	}

    GetObjectRect( obj, &rect);

    if ( obj->flags & OBJ_FLAG_CLIENTSIZE ) {
        AdjustWindowRect( &rect, obj->style, FALSE);
    }

	hwnd = CreateWindowEx( ExStyleFilterForPreview( obj), ((obj->id != CTRL_ID_WINDOW) ? obj->classname : T("DefaultWnd")), obj->title, StyleFilterForPreview( obj),
                           x, y, rect.right - rect.left, rect.bottom - rect.top, hParent, NULL, GetModuleHandle(NULL), NULL);

	if ( !hwnd ) return 0;

	if ( hParent != hMainWnd ) {
		if ( obj->id == CTRL_ID_UPDOWN ) {
			SetWindowPos( hwnd, NULL, 0, 0, obj->width, obj->height, SWP_NOMOVE | SWP_NOZORDER);
		} else if ( obj->id == CTRL_ID_COMBOBOX ) {
			val = twc_GetObjectPropertyVal( obj, COMBOBOX_TYPE);
			if (val->i != 0) SetWindowPos( hwnd, NULL, 0, 0, obj->width, obj->height * 4, SWP_NOMOVE | SWP_NOZORDER);
		}
		SetWindowPos( hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		SendMessage( hwnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), 0);
		InitControlView( obj, hwnd);
	}

    OBJ_LIST_ITERATE_BEGIN( &obj->child_list);
		if ( PreviewWindow( NODE()->elem, hwnd) == 0 ) {
			return 0;
		}
	OBJ_LIST_ITERATE_END();

	return 1;
}