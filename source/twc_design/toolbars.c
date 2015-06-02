#include <windows.h>

#include "twc_design.h"

#include "mainwnd.h"
#include "window.h"
#include "selection.h"
#include "win_stuff.h"

#include "resource.h"

#include "toolbars.h"

#define MAINTLB_FIRST_MESSAGE_CODE (WM_APP + 100)

static void NewWindow();
static void OpenProject();
static void SaveProject();
static void Preview();
static void GenerateCode();
static void AlignLefts();
static void AlignTops();
static void AlignRights();
static void AlignBottoms();
static void CenterVertcally();
static void CenterAcross();
static void HorizontalGap();
static void VerticalGap();
static void MakeSameWidth();
static void MakeSameHeight();
static void MakeSameSize();

typedef struct tagMAINTLB_BTN {
	TCHAR *text;
	UINT img_id;
	void (* proc)();
} MAINTLB_BTN;

static MAINTLB_BTN buttons[] ={
    { T("New Window"),          0,  NewWindow        },
    { T("Open Project"),        1,  OpenProject      },
    { T("Save Project"),        2,  SaveProject      },
    { T("Preview"),             3,  Preview          },
    { T("Generate Code"),       4,  GenerateCode     },
    { T("Align Lefts"),         5,  AlignLefts       },
    { T("Align Tops"),          6,  AlignTops        },
    { T("Align Rights"),        7,  AlignRights      },
    { T("Align Bottoms"),       8,  AlignBottoms     },
    { T("Center Vertically"),   9,  CenterVertcally  },
    { T("Center Across"),       10, CenterAcross     },
    { T("Horizontal Gap"),      11, HorizontalGap    },
    { T("Vertical Gap"),        12, VerticalGap      },
    { T("Make Same Width"),     13, MakeSameWidth    },
    { T("Make Same Height"),    14, MakeSameHeight   },
    { T("Make Same Size"),      15, MakeSameSize     },
    {NULL}
};

static HWND main_toolbar_hwnd;

int AddMainToolbarButtons( HWND hwnd)
{
	TBADDBITMAP tb_ab;
	TBBUTTON tbb;
	TBBUTTON tbb_sep;
	HBITMAP hBmp;
	int i, j;
    MAINTLB_BTN *cur_btn;

	SendMessage( hwnd, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

	hBmp = LoadImage(GetModuleHandle( NULL), MAKEINTRESOURCE( IDB_MAINTLB_ICONS), IMAGE_BITMAP, 256, 16, 0);

	hBmp = MakeBitMapTransparent( hBmp);

	SendMessage( hwnd, TB_SETINDENT, 2, 0);
	SendMessage( hwnd, TB_SETMAXTEXTROWS, 0, 0);

	tb_ab.hInst = NULL;
	tb_ab.nID = (UINT_PTR)hBmp;
	SendMessage( hwnd, TB_ADDBITMAP, 16, (LPARAM)&tb_ab);

	memset( &tbb_sep, 0, sizeof(tbb_sep));
	tbb_sep.fsStyle = BTNS_SEP;

	tbb.fsState = TBSTATE_ENABLED;
	tbb.fsStyle = BTNS_BUTTON;// | BTNS_AUTOSIZE;

	for ( cur_btn = buttons, i = 0, j = 0; cur_btn->text != NULL; i++, j++, cur_btn++ ) {
		tbb.iBitmap = cur_btn->img_id;
		tbb.idCommand = MAINTLB_FIRST_MESSAGE_CODE + j;
		tbb.iString = (INT_PTR)cur_btn->text;
		tbb.dwData = j;

		SendMessage( hwnd, TB_INSERTBUTTON, i, (LPARAM)&tbb);

		if ( j == 2 || j == 4 ) {
			SendMessage( hwnd, TB_INSERTBUTTON, i + 1, (LPARAM)&tbb_sep);
			i++;
		} else if ( j == 8 || j == 10 || j == 12 ) {
			SendMessage( hwnd, TB_INSERTBUTTON, i + 1, (LPARAM)&tbb_sep);
			i++;
		} 
	}

    main_toolbar_hwnd = hwnd;
	UpdateWindow( hwnd);
	return 1;
}

void MainToolbarProc( UINT tb_id)
{
    TBBUTTON tbb;
    UINT code;

    SendMessage( main_toolbar_hwnd, TB_GETBUTTON,
                 SendMessage(main_toolbar_hwnd, TB_COMMANDTOINDEX, tb_id, 0),
                 (LPARAM)&tbb);
	code = tbb.dwData ;

    buttons[code].proc();

	return;
}

static void NewWindow()
{
    AddWindow();
    return;
}

static void OpenProject()
{
    actOpenProject();
    return;
}

static void SaveProject()
{
    actSaveProject();
    return;
}

static void Preview()
{
    actPreview();
    return;
}

static void GenerateCode()
{
    actGenerateCode();
    return;
}

static void AlignLefts()
{
    DLIST_NODE_PTWC_OBJECT *node;
    int i;

    if ( GetSelectedObjects()->count <= 1 ) return;

    node = GetSelectedObjects()->first;
	i = node->elem->x;
    OBJ_LIST_ITERATE_BEGIN_FROM_NODE( node->next);
		SetWindowPos( NODE()->elem->hwnd, NULL, i, NODE()->elem->y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	OBJ_LIST_ITERATE_END();
    return;
}

static void AlignTops()
{
    DLIST_NODE_PTWC_OBJECT *node;
    int i;

    if ( GetSelectedObjects()->count <= 1 ) return;

    node = GetSelectedObjects()->first;
	i = node->elem->y;
	OBJ_LIST_ITERATE_BEGIN_FROM_NODE( node->next);
		SetWindowPos( NODE()->elem->hwnd, NULL, NODE()->elem->x, i, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	OBJ_LIST_ITERATE_END();
    return;
}

static void AlignRights()
{
    DLIST_NODE_PTWC_OBJECT *node;
    int i;

    if (GetSelectedObjects()->count <= 1) return;

    node = GetSelectedObjects()->first;
	i = node->elem->x + node->elem->width;
	OBJ_LIST_ITERATE_BEGIN_FROM_NODE( node->next);
		SetWindowPos( NODE()->elem->hwnd, NULL, i - NODE()->elem->width, NODE()->elem->y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    OBJ_LIST_ITERATE_END();
    return;
}

static void AlignBottoms()
{
    DLIST_NODE_PTWC_OBJECT *node;
    int i;

    if ( GetSelectedObjects()->count <= 1 ) return;

	node = GetSelectedObjects()->first;
	i = node->elem->y + node->elem->height;
	OBJ_LIST_ITERATE_BEGIN_FROM_NODE( node->next);
		SetWindowPos( NODE()->elem->hwnd, NULL, NODE()->elem->x, i - NODE()->elem->height, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	OBJ_LIST_ITERATE_END();
    return;
}

static void CenterVertcally()
{
    DLIST_NODE_PTWC_OBJECT *node;
    RECT rect;
    int i, j, delta;

    if ( GetSelectedObjects()->count == 0 ) return;

	node = GetSelectedObjects()->first;
	i = j = node->elem->y;
	OBJ_LIST_ITERATE_BEGIN_FROM_NODE( node);
        if ( NODE()->elem->y < i ) {
            i = NODE()->elem->y;
        }
        if ( NODE()->elem->y + NODE()->elem->height > j ) {
            j = NODE()->elem->y + NODE()->elem->height;
        }
	OBJ_LIST_ITERATE_END();

	GetClientRect( GetSelectedObjects()->first->elem->parent->hwnd, &rect);
	delta = (i + j) / 2 - (rect.top + rect.bottom) / 2;
	OBJ_LIST_ITERATE_BEGIN( GetSelectedObjects());
		SetWindowPos( NODE()->elem->hwnd, NULL, NODE()->elem->x, NODE()->elem->y - delta, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	OBJ_LIST_ITERATE_END();
    return;
}

static void CenterAcross()
{
    DLIST_NODE_PTWC_OBJECT *node;
    RECT rect;
    int i, j, delta;

    if ( GetSelectedObjects()->count == 0 ) return;

	node = GetSelectedObjects()->first;
	i = j = node->elem->x;
	OBJ_LIST_ITERATE_BEGIN_FROM_NODE( node);
        if ( NODE()->elem->x < i ) {
            i = NODE()->elem->x;
        }
        if ( NODE()->elem->x + NODE()->elem->width > j ) {
            j = NODE()->elem->x + NODE()->elem->width;
        }
	OBJ_LIST_ITERATE_END();

	GetClientRect( GetSelectedObjects()->first->elem->parent->hwnd, &rect);
	delta = (i + j) / 2 - (rect.right + rect.left) / 2;
	OBJ_LIST_ITERATE_BEGIN( GetSelectedObjects());
		SetWindowPos( NODE()->elem->hwnd, NULL, NODE()->elem->x - delta, NODE()->elem->y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	OBJ_LIST_ITERATE_END();
    return;
}

static void HorizontalGap()
{
    DLIST_NODE_PTWC_OBJECT *node;
    int i, j, delta, sum;

    if ( GetSelectedObjects()->count <= 2 ) return;

	node = GetSelectedObjects()->first;
	i = j = node->elem->x;
	sum = 0;
	OBJ_LIST_ITERATE_BEGIN_FROM_NODE( node);
        if ( NODE()->elem->x < i ) {
            i = NODE()->elem->x;
        }
        if ( NODE()->elem->x + NODE()->elem->width > j ) {
            j = NODE()->elem->x + NODE()->elem->width;
        }
		sum += NODE()->elem->width;
	OBJ_LIST_ITERATE_END();

	delta = j - i;
	sum = ((delta - sum) > 0) ? ((delta - sum) / (GetSelectedObjects()->count - 1)) : -(int)((sum - delta) / (GetSelectedObjects()->count - 1));
	OBJ_LIST_ITERATE_BEGIN( GetSelectedObjects());
		SetWindowPos( NODE()->elem->hwnd, NULL, i, NODE()->elem->y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		i += NODE()->elem->width + sum;
	OBJ_LIST_ITERATE_END();
    return;
}

static void VerticalGap()
{
    DLIST_NODE_PTWC_OBJECT *node;
    int i, j, delta, sum;

    if ( GetSelectedObjects()->count <= 2 ) return;

	node = GetSelectedObjects()->first;
	i = j = node->elem->y;
	sum = 0;
	OBJ_LIST_ITERATE_BEGIN_FROM_NODE( node);
        if ( NODE()->elem->y < i ) {
            i = NODE()->elem->y;
        }
        if ( NODE()->elem->y + NODE()->elem->height > j ) {
            j = NODE()->elem->y + NODE()->elem->height;
        }
		sum += NODE()->elem->height;
	OBJ_LIST_ITERATE_END();

	delta = j - i;
	sum = ((delta - sum) > 0) ? ((delta - sum) / (GetSelectedObjects()->count - 1)) : -(int)((sum - delta) / (GetSelectedObjects()->count - 1));
	OBJ_LIST_ITERATE_BEGIN( GetSelectedObjects());
		SetWindowPos( NODE()->elem->hwnd, NULL, NODE()->elem->x, i, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		i += NODE()->elem->height + sum;
	OBJ_LIST_ITERATE_END();
    return;
}

static void MakeSameWidth()
{
    DLIST_NODE_PTWC_OBJECT *node;
    int i;

    if ( GetSelectedObjects()->count <= 1 ) return;

	node = GetSelectedObjects()->first;
	i = node->elem->width;
	OBJ_LIST_ITERATE_BEGIN_FROM_NODE( node->next);
		SetWindowPos( NODE()->elem->hwnd, NULL, NODE()->elem->x, NODE()->elem->y, i, NODE()->elem->height, SWP_NOMOVE | SWP_NOZORDER);
	OBJ_LIST_ITERATE_END();
    return;
}

static void MakeSameHeight()
{
    DLIST_NODE_PTWC_OBJECT *node;
    int j;

    if ( GetSelectedObjects()->count <= 1 ) return;

	node = GetSelectedObjects()->first;
	j = node->elem->height;
	OBJ_LIST_ITERATE_BEGIN_FROM_NODE( node->next);
		SetWindowPos( NODE()->elem->hwnd, NULL, NODE()->elem->x, NODE()->elem->y, NODE()->elem->width, j, SWP_NOMOVE | SWP_NOZORDER);
    OBJ_LIST_ITERATE_END();
    return;
}

static void MakeSameSize()
{
    DLIST_NODE_PTWC_OBJECT *node;
    int i, j;

    if ( GetSelectedObjects()->count <= 1 ) return;

	node = GetSelectedObjects()->first;
	i = node->elem->width;
	j = node->elem->height;
	OBJ_LIST_ITERATE_BEGIN_FROM_NODE( node->next);
		SetWindowPos( NODE()->elem->hwnd, NULL, NODE()->elem->x, NODE()->elem->y, i, j, SWP_NOMOVE | SWP_NOZORDER);
	OBJ_LIST_ITERATE_END();
    return;
}
