#include <windows.h>
#include "twc.h"

typedef struct tagTWC_OBJECT {
	TCHAR *classname;
	int x, y;
	int width, height;
	TCHAR *title;
	DWORD style;
	DWORD exstyle;

	struct tagTWC_OBJECT **child_list;
	struct tagTWC_OBJECT *parent;

	HWND hwnd;
	HWND hParent
} TWC_OBJECT;




TWC_OBJECT MainWnd_btnStart = {
	BUTTON_CLASS,
	5, 5,
	50, 20,
	TEXT("PWNZ"),
	WS_VISIBLE | WS_CHILD,
	0,
	NULL,
	NULL
};

TWC_OBJECT *MainWnd_childs[] = {
	&MainWnd_btnStart,
	NULL
};

TWC_OBJECT MainWnd = {
	TEXT("DefaultWnd"),
	20, 30,
	230, 150,
	TEXT("VSEMQ"),
	WS_VISIBLE | WS_CLIPCHILDREN, WS_OVERLAPPED,
	0,
	MainWnd_childs,
	NULL
};