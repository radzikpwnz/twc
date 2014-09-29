#include <windows.h>

#include "twc_design.h"

#include "control.h"
#include "selection.h"
#include "toolbox.h"
#include "control_act.h"

HCURSOR arrow_cursor, lr_cursor, tb_cursor, lt_cursor, tr_cursor;

int LoadControlCursors()
{
    arrow_cursor = LoadCursor( NULL, IDC_ARROW);
    lr_cursor = LoadCursor( NULL, IDC_SIZEWE);
    tb_cursor = LoadCursor( NULL, IDC_SIZENS);
    lt_cursor = LoadCursor( NULL, IDC_SIZENWSE);
    tr_cursor = LoadCursor( NULL, IDC_SIZENESW);
	return 1;
}

static int PaintControlSelectionFrame( RT_OBJECT *obj)
{
    PAINTSTRUCT ps;
    HDC hDC;
    HBRUSH hOldBrush;
    HPEN hPen, hOldPen;
    LOGBRUSH lb;
    RECT rect;

    hDC = BeginPaint( obj->static_hwnd, &ps);

    if ( obj->selected ) {
        hDC = GetWindowDC( obj->static_hwnd);
        GetWindowRect( obj->static_hwnd, &rect);
        MapWindowPoints( HWND_DESKTOP, obj->static_hwnd, (POINT *)&rect, 2);

        hOldBrush = SelectObject( hDC, GetStockObject( NULL_BRUSH));

        lb.lbStyle = BS_SOLID;
        lb.lbColor = RGB(0, 130, 255); 
        lb.lbHatch = 0;
        hPen = ExtCreatePen( PS_GEOMETRIC | PS_DASH, 2, &lb, 0, NULL);
        hOldPen = SelectObject( hDC, hPen);

        Rectangle( hDC, rect.left + 1, rect.top + 1, rect.right, rect.bottom);
        SelectObject( hDC, hOldBrush);
        SelectObject( hDC, hOldPen);
        DeleteObject( hPen);
        ReleaseDC( obj->static_hwnd, hDC);
    }

    EndPaint( obj->static_hwnd, &ps);

    return 1;
}

LRESULT CALLBACK StaticWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    RT_OBJECT *obj;

	static struct {
		int left, top, right, bottom;
	} dir, dir_tmp;
	static POINTS prev;
	static HWND hPrevCapture;
	static int deltax, deltay;

	static int sizing = 0;
    static int moving = 0;

    RECT rect;
	POINTS p;
	HCURSOR hCursor;
	POINT point;
	int dx, dy;


    obj = GetProp( hwnd, T("OBJECT_INFO"));

    switch ( msg ) {
        case WM_NCDESTROY:
            RemoveProp( hwnd, T("OBJECT_INFO"));
            obj->static_hwnd = NULL;
            break;
        case WM_SETCURSOR:
            return 1;
        case WM_PAINT:
            PaintControlSelectionFrame( obj);
            return 0;
       case WM_LBUTTONDOWN:
			if (GetAsyncKeyState(VK_CONTROL) >> (sizeof(SHORT) - 1)) {
				if ( !obj->selected ) {
					AddObjectToSelection( obj);
					if ( GetSelectedObjects()->count == 1) {
						SetCurrentObject( obj);
					} else {
						SetCurrentObject( obj->parent);
					}
				} else {
                    DeleteObjectFromSelection( obj);
                    if ( GetSelectedObjects()->count == 1 ) {
                        SetCurrentObject( GetSelectedObjects()->first->elem);
                    } else if ( GetSelectedObjects()->count == 0 ) {
                        SetCurrentObject( obj->parent);
                    }
				}
                RedrawWindow( hwnd, NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE | RDW_FRAME);
				return 0;
			}

            if ( !obj->selected ) {
                ClearSelection();
                AddObjectToSelection( obj);
                SetCurrentObject( obj);
            }
			
			ReleaseToolboxElemSelection();

			if ( obj->id == CTRL_ID_STATUSBAR ) return 0; //TODO: fix

			if ( new_control ) {
				new_control = 0;
				dir.right = dir.bottom = 1;
			} else {
				GetWindowRect( hwnd, &rect);
				MapWindowPoints( HWND_DESKTOP, hwnd, (POINT *)&rect, 2);

				prev = MAKEPOINTS( lParam);

				(prev.x > rect.right - 5) && (prev.x < rect.right) && (dir.right = 1) && (deltax = rect.right - prev.x);
				if (!dir.right) {
					(prev.x < rect.left + 5) && (prev.x >= rect.left) && (dir.left = 1) && (deltax = prev.x - rect.left);
				}
				(prev.y > rect.bottom - 5) && (prev.y < rect.bottom) && (dir.bottom = 1) && (deltay = rect.bottom - prev.y);
				if (!dir.bottom) {
					(prev.y < rect.top + 5) &&  (prev.y >= rect.top) && (dir.top = 1) && (deltay = prev.y - rect.top);
				}
			}

			if ((dir.left || dir.top || dir.right || dir.bottom) && GetSelectedObjects()->count == 1) {
				sizing = 1;
			} else {
				moving = 1;
			}
			ReleaseCapture();
			hPrevCapture = SetCapture( hwnd);
			return 0;
		case WM_MOUSEMOVE:
			p = MAKEPOINTS(lParam);
			if ( !(wParam & MK_LBUTTON) && GetSelectedObjects()->count <= 1) {
				GetWindowRect(hwnd, &rect);
				MapWindowPoints(HWND_DESKTOP, hwnd, (POINT *)&rect, 2);

				dir_tmp.left = dir_tmp.top = dir_tmp.right = dir_tmp.bottom = 0;
				(p.x < rect.left + 5) && (dir_tmp.left = 1);
				(p.x > rect.right - 5) && (dir_tmp.right = 1);
				(p.y < rect.top + 5) && (dir_tmp.top = 1);
				(p.y > rect.bottom - 5) && (dir_tmp.bottom = 1) ;

				if (dir_tmp.left || dir_tmp.right) {
					if (dir_tmp.top || dir_tmp.bottom) {
						if (dir_tmp.left && dir_tmp.top || dir_tmp.right && dir_tmp.bottom) {
							hCursor = lt_cursor;
						} else {
							hCursor = tr_cursor;
						}
					} else {
						hCursor = lr_cursor;
					}
				} else if (dir_tmp.top || dir_tmp.bottom) {
					hCursor = tb_cursor;
				} else {
					hCursor = arrow_cursor;
				}

				SetCursor(hCursor);
			} else {
				if (sizing) {
					GetWindowRect( hwnd, &rect);
					MapWindowPoints( HWND_DESKTOP, GetParent(hwnd), (LPPOINT)&rect, 2);

					point.x = p.x;
					point.y = p.y;
					MapWindowPoints( hwnd, GetParent(hwnd), &point, 1);

					if (dir.right && (point.x + deltax >= rect.left + CONTROL_MIN_WIDTH) && ((point.x + deltax) % grid_size == 0)) rect.right = point.x + deltax;
					if (dir.bottom && (point.y + deltay >= rect.top + CONTROL_MIN_WIDTH) && ((point.y + deltay) % grid_size == 0)) 	rect.bottom = point.y+ deltay;
					if (dir.left && (point.x - deltax <= rect.right - CONTROL_MIN_WIDTH) && ((point.x - deltax) % grid_size == 0)) rect.left = point.x - deltax;
					if (dir.top && (point.y - deltay <= rect.bottom - CONTROL_MIN_HEIGHT) && ((point.y - deltay) % grid_size == 0)) rect.top = point.y - deltay;

					MoveWindow( obj->hwnd, rect.left, rect.top, (rect.right - rect.left), (rect.bottom - rect.top), 1);
				} else if ( moving ) {
					dx = p.x - prev.x;
					dy = p.y - prev.y;

					if ( (dx && dx % grid_size == 0) || (dy && dy % grid_size == 0) ) {
                        MoveSelectedObjects( RoundByGrid(dx), RoundByGrid(dy));
					}
				}
			}
			return 0;
		case WM_LBUTTONUP:
			if (sizing) {
				sizing = 0;
				dir.left = dir.top = dir.right = dir.bottom = 0;
				if (obj->id == CTRL_ID_LISTBOX && !(obj->style & LBS_NOINTEGRALHEIGHT)) {
					GetWindowRect(hwnd, &rect);
					MapWindowPoints( HWND_DESKTOP, GetParent(hwnd), (LPPOINT)&rect, 2);
					rect.bottom -= rect.bottom % SendMessage(obj->hwnd, LB_GETITEMHEIGHT, 0, 0);
					SetWindowPos( obj->hwnd, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER | SWP_NOMOVE);
				}
				ReleaseCapture();
				if (hPrevCapture) SetCapture(hPrevCapture);
			} else if (moving) {
				moving = 0;
				ReleaseCapture();
				if (hPrevCapture) SetCapture(hPrevCapture);
			}
			InvalidateRect(GetParent(hwnd), NULL, 0);
			return 0;
        case WM_NCHITTEST:
            return HTCLIENT;
    }

    return CallWindowProc( obj->static_orig_wndproc, hwnd, msg, wParam, lParam);
}