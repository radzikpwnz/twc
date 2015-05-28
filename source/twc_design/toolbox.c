#include <windows.h>

#include "twc_design.h"

#include "win_stuff.h"

#include "resource.h"

#include "toolbox.h"


#define TB_FIRST_MESSAGE_CODE	(WM_APP)

typedef struct tagTOOLBOX_BTN {
	TCHAR *text;
	UINT ctrl_id;
	UINT img_id;
} TOOLBOX_BTN;

static TOOLBOX_BTN buttons[] = {
	{ T("Button"),           CTRL_ID_BUTTON,       0 },
	{ T("Check box"),        CTRL_ID_CHECKBOX,     1 },
	{ T("Radio button"),     CTRL_ID_RADIOBUTTON,  2 },
	{ T("Group box"),        CTRL_ID_GROUPBOX,     3 },
	{ T("Static"),           CTRL_ID_STATIC,       4 },
	{ T("Edit control"),     CTRL_ID_EDIT,         5 },
	{ T("List box"),         CTRL_ID_LISTBOX,      6 },
	{ T("Combo box (TEST)"), CTRL_ID_COMBOBOX,     7 },
	{ T("Slider control"),   CTRL_ID_SLIDER,       8 },
	{ T("Progress bar"),     CTRL_ID_PROGRESSBAR,  9 },
	{ T("IP address"),       CTRL_ID_IPADDRESS,    10 },
	{ T("Datetime Picker"),  CTRL_ID_DATETIME,     11 },
	{ T("Scroll bar"),       CTRL_ID_SCROLLBAR,    12 },
	{ T("Up-Down control"),  CTRL_ID_UPDOWN,       13 },
	{ T("Status bar"),       CTRL_ID_STATUSBAR,    14 },
	{ T("Toolbar"),          CTRL_ID_TOOLBAR,      15 },	
	{NULL}
};

static HWND tb_hwnd;

static UINT current_control_id;
static UINT tb_prev_id;
static BOOL is_tmpl_valid = TWC_FALSE;

int AddToolboxButtons( HWND hwnd)
{
    TBADDBITMAP tb_ab;
	TBBUTTON tbb;
	TOOLBOX_BTN *cur_btn;
    HBITMAP hBmp;
	UINT i;

	SendMessage( hwnd, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

    hBmp = LoadImage( GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_CONTROLS_ICONS), IMAGE_BITMAP, 256, 16, 0);
	hBmp = MakeBitMapTransparent( hBmp);

    tb_ab.hInst = NULL;
	tb_ab.nID = (UINT_PTR)hBmp;
	SendMessage( hwnd, TB_ADDBITMAP, 16, (LPARAM)&tb_ab);

	memset( &tbb, 0, sizeof(tbb));
	tbb.fsState = TBSTATE_ENABLED | TBSTATE_WRAP;
	tbb.fsStyle = BTNS_BUTTON | BTNS_CHECK;

    for ( cur_btn = buttons, i = 0; cur_btn->text != NULL; i++, cur_btn++ ) {
		tbb.iBitmap = cur_btn->img_id;
		tbb.idCommand = cur_btn->ctrl_id + TB_FIRST_MESSAGE_CODE;
		tbb.iString = (INT_PTR)cur_btn->text;
		tbb.dwData = i;
		
		SendMessage( hwnd, TB_INSERTBUTTON, i, (LPARAM)&tbb);
	}

	tb_hwnd = hwnd;
	UpdateWindow( hwnd);
	return 1;
}

UINT GetCurrentToolboxControlId()
{
    if ( is_tmpl_valid ) {
        return current_control_id;
    } else {
        return CTRL_ID_UNDEFINED;
    }
}

BOOL IsToolboxControlSelected()
{
    return is_tmpl_valid;
}

int ProcessToolboxElemSelection( UINT tb_id)
{
    TBBUTTON tbb;
	int i;

	if ( tb_prev_id ) {
		if ( tb_prev_id == tb_id ) {
            ReleaseToolboxElemSelection();
            return 1;
        } else {
			SendMessage( tb_hwnd, TB_CHECKBUTTON, tb_prev_id, 0);
		}
	}
	
    SendMessage( tb_hwnd, TB_COMMANDTOINDEX, tb_id, 0);
    SendMessage( tb_hwnd, TB_GETBUTTON,
                 SendMessage( tb_hwnd, TB_COMMANDTOINDEX, tb_id, 0),
                 (LPARAM)&tbb);
	i = tbb.dwData ;

	current_control_id =  buttons[i].ctrl_id;

    SendMessage( tb_hwnd, TB_CHECKBUTTON, tb_id, 1);
    is_tmpl_valid = TWC_TRUE;

	tb_prev_id = tb_id;

    return 1;
}

int ReleaseToolboxElemSelection()
{
    if ( tb_prev_id ) {
        SendMessage( tb_hwnd, TB_CHECKBUTTON, tb_prev_id, 0);
        tb_prev_id = 0;
    }

    is_tmpl_valid = TWC_FALSE;

    return 1;
}