#ifndef TWC_H
#define TWC_H

#include <CommCtrl.h>

#define TWC_BUTTON_CLASS        WC_BUTTON
#define TWC_CHECKBOX_CLASS      WC_BUTTON
#define TWC_RADIOBUTTON_CLASS   WC_BUTTON
#define TWC_EDIT_CLASS          WC_EDIT
#define TWC_LISTBOX_CLASS       WC_LISTBOX
#define TWC_COMBOBOX_CLASS      WC_COMBOBOX
#define TWC_STATIC_CLASS        WC_STATIC
#define TWC_GROUPBOX_CLASS      WC_BUTTON
#define TWC_SLIDER_CLASS        TRACKBAR_CLASS
#define TWC_PROGRESSBAR_CLASS   PROGRESS_CLASS
#define TWC_IPADDRESS_CLASS     WC_IPADDRESS
#define TWC_DATETIME_CLASS      DATETIMEPICK_CLASS
#define TWC_SCROLLBAR_CLASS     WC_SCROLLBAR
#define TWC_UPDOWN_CLASS        UPDOWN_CLASS
#define TWC_STATUSBAR_CLASS     STATUSCLASSNAME
#define TWC_TOOLBAR_CLASS       TOOLBARCLASSNAME

//#define TWC_RICHEDIT_CLASS    MSFTEDIT_CLASS

#define TWC_MDICLIENT_CLASS		TEXT("MDICLIENT")


enum {
    TWC_OBJ_FLAG_CLIENTSIZE = 1 << 0
};

typedef struct tagTWC_OBJECT {
	TCHAR *classname;
	int x, y;
	int width, height;
	TCHAR *title;
	DWORD style;
	DWORD exstyle;

    UINT flags;

	struct tagTWC_OBJECT **child_list;
	struct tagTWC_OBJECT *parent;

	HWND hwnd;
	HWND hParent;
} TWC_OBJECT;


int twcCreateObject(TWC_OBJECT *obj, int create_childs, HWND hParent);
int twcInit();

#endif