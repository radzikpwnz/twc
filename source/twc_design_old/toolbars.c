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

static HWND tb_hwnd;

int AddMainToolbarButtons(HWND hwnd)
{
	TBADDBITMAP tb_ab;
	TBBUTTON tbb;
	TBBUTTON tbb_sep;
	HBITMAP hBmp;
	int i, j;
    MAINTLB_BTN *cur_btn;

	SendMessage(hwnd, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

	hBmp = LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_MAINTLB_ICONS), IMAGE_BITMAP, 256, 16, 0);

	hBmp = MakeBitMapTransparent(hBmp);

	SendMessage(hwnd, TB_SETINDENT, 2, 0);
	SendMessage(hwnd, TB_SETMAXTEXTROWS, 0, 0);

	tb_ab.hInst = NULL;
	tb_ab.nID = (UINT_PTR)hBmp;
	SendMessage(hwnd, TB_ADDBITMAP, 16, (LPARAM)&tb_ab);

	memset(&tbb_sep, 0, sizeof(tbb_sep));
	tbb_sep.fsStyle = BTNS_SEP;

	tbb.fsState = TBSTATE_ENABLED;
	tbb.fsStyle = BTNS_BUTTON;// | BTNS_AUTOSIZE;

	for ( cur_btn = buttons, i = 0, j = 0; cur_btn->text != NULL; i++, j++, cur_btn++ ) {
		tbb.iBitmap = cur_btn->img_id;
		tbb.idCommand = MAINTLB_FIRST_MESSAGE_CODE + j;
		tbb.iString = (INT_PTR)cur_btn->text;
		tbb.dwData = j;

		SendMessage(hwnd, TB_INSERTBUTTON, i, (LPARAM)&tbb);

		if (j == 2 || j == 4) {
			SendMessage(hwnd, TB_INSERTBUTTON, i + 1, (LPARAM)&tbb_sep);
			i++;
		} else if (j == 8 || j == 10 || j == 12) {
			SendMessage(hwnd, TB_INSERTBUTTON, i + 1, (LPARAM)&tbb_sep);
			i++;
		} 
	}

    tb_hwnd = hwnd;
	UpdateWindow(hwnd);
	return 1;
}

void MainToolbarProc( UINT tb_id)
{
    TBBUTTON tbb;
    UINT code;

    SendMessage( tb_hwnd, TB_GETBUTTON,
                 SendMessage(tb_hwnd, TB_COMMANDTOINDEX, tb_id, 0),
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
    DLIST_NODE_PRT_OBJECT *node;
    int i;

    if (GetSelectedObjects()->count <= 1) return;
	(void *)node = (void *)GetSelectedObjects()->first;
	if (node) i = node->elem->x;
	while (node != NULL) {
		SetWindowPos(node->elem->hwnd, NULL, i, node->elem->y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		node = node->next;
	}
    return;
}

static void AlignTops()
{
    DLIST_NODE_PRT_OBJECT *node;
    int i;

    if (GetSelectedObjects()->count <= 1) return;
	(void *)node = (void *)GetSelectedObjects()->first;
	if (node) i = node->elem->y;
	while (node != NULL) {
		SetWindowPos(node->elem->hwnd, NULL, node->elem->x, i, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		node = node->next;
	}
    return;
}

static void AlignRights()
{
    DLIST_NODE_PRT_OBJECT *node;
    int i;

    if (GetSelectedObjects()->count <= 1) return;
	(void *)node = (void *)GetSelectedObjects()->first;
	if (node) i = node->elem->x + node->elem->width;
	while (node != NULL) {
		SetWindowPos(node->elem->hwnd, NULL, i - node->elem->width, node->elem->y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		node = node->next;
	}
    return;
}

static void AlignBottoms()
{
    DLIST_NODE_PRT_OBJECT *node;
    int i;

    if (GetSelectedObjects()->count <= 1) return;
	(void *)node = (void *)GetSelectedObjects()->first;
	if (node) i = node->elem->y + node->elem->height;
	while (node != NULL) {
		SetWindowPos(node->elem->hwnd, NULL, node->elem->x, i - node->elem->height, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		node = node->next;
	}
    return;
}

static void CenterVertcally()
{
    DLIST_NODE_PRT_OBJECT *node;
    RECT rect;
    int i, j, delta;

    if (GetSelectedObjects()->count == 0) return;
	(void *)node = (void *)GetSelectedObjects()->first;
	i = j = node->elem->y;
	while (node != NULL) {
		if (node->elem->y < i) i = node->elem->y;
		if (node->elem->y + node->elem->height > j) j = node->elem->y + node->elem->height;
		node = node->next;
	}
	GetClientRect(GetSelectedObjects()->first->elem->parent->hwnd, &rect);
	delta = (i + j) / 2 - (rect.top + rect.bottom) / 2;
	(void *)node = (void *)GetSelectedObjects()->first;
	while (node != NULL) {
		SetWindowPos(node->elem->hwnd, NULL, node->elem->x, node->elem->y - delta, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		node = node->next;
	}
    return;
}

static void CenterAcross()
{
    DLIST_NODE_PRT_OBJECT *node;
    RECT rect;
    int i, j, delta;

    if (GetSelectedObjects()->count == 0) return;
	(void *)node = (void *)GetSelectedObjects()->first;
	i = j = node->elem->x;
	while (node != NULL) {
		if (node->elem->x < i) i = node->elem->x;
		if (node->elem->x + node->elem->width > j) j = node->elem->x + node->elem->width;
		node = node->next;
	}
	GetClientRect(GetSelectedObjects()->first->elem->parent->hwnd, &rect);
	delta = (i + j) / 2 - (rect.right + rect.left) / 2;
	(void *)node = (void *)GetSelectedObjects()->first;
	while (node != NULL) {
		SetWindowPos(node->elem->hwnd, NULL, node->elem->x - delta, node->elem->y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		node = node->next;
	}
    return;
}

static void HorizontalGap()
{
    DLIST_NODE_PRT_OBJECT *node;
    int i, j, delta, sum;

    if (GetSelectedObjects()->count <= 2) return;
	(void *)node = (void *)GetSelectedObjects()->first;
	i = j = node->elem->x;
	sum = 0;
	while (node != NULL) {
		if (node->elem->x < i) i = node->elem->x;
		if (node->elem->x + node->elem->width > j) j = node->elem->x + node->elem->width;
		sum += node->elem->width;
		node = node->next;
	}
	delta = j - i;
	sum = ((delta - sum) > 0) ? ((delta - sum) / (GetSelectedObjects()->count - 1)) : -(int)((sum - delta) / (GetSelectedObjects()->count - 1));
	(void *)node = (void *)GetSelectedObjects()->first;
	while (node != NULL) {
		SetWindowPos(node->elem->hwnd, NULL, i, node->elem->y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		i += node->elem->width + sum;
		node = node->next;
	}
    return;
}

static void VerticalGap()
{
    DLIST_NODE_PRT_OBJECT *node;
    int i, j, delta, sum;

    if (GetSelectedObjects()->count <= 2) return;
	(void *)node = (void *)GetSelectedObjects()->first;
	i = j = node->elem->y;
	sum = 0;
	while (node != NULL) {
		if (node->elem->y < i) i = node->elem->y;
		if (node->elem->y + node->elem->height > j) j = node->elem->y + node->elem->height;
		sum += node->elem->height;
		node = node->next;
	}
	delta = j - i;
	sum = ((delta - sum) > 0) ? ((delta - sum) / (GetSelectedObjects()->count - 1)) : -(int)((sum - delta) / (GetSelectedObjects()->count - 1));
	(void *)node = (void *)GetSelectedObjects()->first;
	while (node != NULL) {
		SetWindowPos(node->elem->hwnd, NULL, node->elem->x, i, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		i += node->elem->height + sum;
		node = node->next;
	}
    return;
}

static void MakeSameWidth()
{
    DLIST_NODE_PRT_OBJECT *node;
    int i;

    if (GetSelectedObjects()->count <= 1) return;
	(void *)node = (void *)GetSelectedObjects()->first;
	i = node->elem->width;
	while (node != NULL) {
		SetWindowPos(node->elem->hwnd, NULL, node->elem->x, node->elem->y, i, node->elem->height, SWP_NOMOVE | SWP_NOZORDER);
		node = node->next;
	}
    return;
}

static void MakeSameHeight()
{
    DLIST_NODE_PRT_OBJECT *node;
    int j;

    if (GetSelectedObjects()->count <= 1) return;
	(void *)node = (void *)GetSelectedObjects()->first;
	j = node->elem->height;
	while (node != NULL) {
		SetWindowPos(node->elem->hwnd, NULL, node->elem->x, node->elem->y, node->elem->width, j, SWP_NOMOVE | SWP_NOZORDER);
		node = node->next;
	}
    return;
}

static void MakeSameSize()
{
    DLIST_NODE_PRT_OBJECT *node;
    int i, j;

    if (GetSelectedObjects()->count <= 1) return;
	(void *)node = (void *)GetSelectedObjects()->first;
	i = node->elem->width;
	j = node->elem->height;
	while (node != NULL) {
		SetWindowPos(node->elem->hwnd, NULL, node->elem->x, node->elem->y, i, j, SWP_NOMOVE | SWP_NOZORDER);
		node = node->next;
	}
    return;
}
