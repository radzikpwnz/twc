#ifndef PROPLIST_H
#define PROPLIST_H

enum {
	PL_ADDITEM = WM_USER,
	PL_DELETEITEM,
	PL_CLEAN,
	PL_SETITEMHEIGHT,
	PL_SETITEMVALUE,
	PL_SETITEMTYPE,
	PL_GETITEMVALUE,

	PL_UPDATESCROLL,
	PL_STARTFILL,
	PL_ENDFILL,
	PL_PROCESSCHANGEDPROP
};

enum {
	PLN_ITEMCHANGE = WM_USER,
};

typedef union tagPL_VALUE{
	HANDLE h;
	TCHAR *s;
	int i;
} PL_VALUE;

typedef enum tagPL_VALTYPE {
	PL_TEXT, //val.s - string (STORED)
	PL_BOOL, //val.i - 1 or 0
	PL_LIST, //val.h = pointer to string list, terminated by NULL (NOT STORED!), LOWORD(lParam) - selected item
	PL_INT //val.i = integer
} PL_VALTYPE;

typedef struct tagPL_ITEMINFO {
	TCHAR *name;
	int id;
	PL_VALTYPE type;
	PL_VALUE val;
} PL_ITEMINFO;

#ifndef PROPLIST_DLL
int InitPropertyListDll(HINSTANCE hInstance);
TCHAR *GetPropListClassname();
#endif


#endif