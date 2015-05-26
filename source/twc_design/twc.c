#include <windows.h>
#include <commctrl.h>

#include "twc.h"

HINSTANCE hThisInstance;
UINT default_font;

CLIENTCREATESTRUCT css;

int twcInit()
{
	hThisInstance = GetModuleHandle(NULL);
	InitCommonControls();
	default_font = (UINT)GetStockObject(DEFAULT_GUI_FONT);
	return 1;
}

int twcCreateObject(TWC_SOBJECT *obj, int create_childs, HWND hParent)
{
	TWC_SOBJECT **ppobj;
	int style;
    RECT rect;

	style = obj->style;
	if (hParent) style |= WS_CHILD;
	obj->hParent = hParent;

    rect.left = obj->x;
    rect.right = obj->x + obj->width;
    rect.top = obj->y;
    rect.bottom = obj->y + obj->height;

    if ( obj->flags & TWC_OBJ_FLAG_CLIENTSIZE ) {
        AdjustWindowRect( &rect, obj->style, FALSE);
    }

	obj->hwnd = CreateWindowEx( obj->exstyle, obj->classname, obj->title, style, \
                                rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, obj->hParent, NULL, hThisInstance, &css);
	if (!obj->hwnd) return 0;
	SendMessage(obj->hwnd, WM_SETFONT, default_font, 0);

	if (!create_childs) return 1;
  ppobj = obj->child_list;
	if (!ppobj) return 1;
	while (*ppobj) {
		if (twcCreateObject(*ppobj, 1, obj->hwnd) == 0) return 0;
		SetWindowPos((*ppobj)->hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		MoveWindow((*ppobj)->hwnd, (*ppobj)->x, (*ppobj)->y, (*ppobj)->width, (*ppobj)->height, 0);
		ppobj++;
	}
	return 1;
}