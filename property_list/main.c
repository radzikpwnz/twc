#pragma comment(linker, "/MERGE:.data=.text") //Объединение секций PE файла для уменьшения размера
#pragma comment(linker, "/MERGE:.rdata=.data")
#pragma comment(linker, "/SECTION:.text,EWR")

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS
#define _USE_MATH_DEFINES

#include <windows.h>
#include <tchar.h>
#include <CommCtrl.h>

#include "mylist.h"
#include "proplist.h"

#define sqr(x) ((x) * (x))
#define MIN(x,y) (((x)>(y))?(y):(x))
#define MAX(x,y) (((x)>(y))?(x):(y))

typedef struct tagITEM {
	HWND hLeft, hRight;

	TCHAR *name;
	int id;
	PL_VALTYPE type;
	PL_VALUE val;
	PL_VALUE prev_val;
	//DLIST(TCHAR *) val_list;
	void *lstnode_ptr;
} ITEM;

typedef struct tagOBJECT {
	int x, y;
	int width, height;
	TCHAR *title;
	DWORD style;
	DWORD exstyle;

	HWND hwnd;
	HWND hParent;
	HFONT hFont;

	int item_height;
	int item_count;
	DLIST(ITEM) item_lst;

	int filling;
	ITEM *cur_item;
} OBJECT;

TCHAR strTRUE[] = TEXT("True");
TCHAR strFALSE[] = TEXT("False");

LRESULT CALLBACK MainWindowProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK MyEditProc(HWND, UINT, WPARAM, LPARAM);

int ProcessChangedItem(OBJECT *obj, ITEM *item);
void Resize(OBJECT *obj, int w, int h);
void PaintMainWnd(HWND hwnd, OBJECT *obj, HDC hDC);

void *SearchItem(OBJECT *obj, int id);
void *SearchItemHWND(OBJECT *obj, HWND hRight);

void FreeObject(OBJECT *obj);
void CleanList(OBJECT *obj);
void DeleteItem(OBJECT *obj, int id);

HINSTANCE hThisInstance;

WNDPROC DefEditProc;


WNDCLASSEX proplist_class = {sizeof (WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, MainWindowProc, 0, 0, 0, 0, 0, NULL, NULL, TEXT("PropertyList"), 0};

void myfree(void **p)
{
	if (*p) free(*p);
	*p = NULL;
}


#define free(x) myfree(&(x))

BOOL WINAPI DllMain(HINSTANCE hinstDLL,  DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH) {
		//MessageBox(NULL, L"Q!", L"PropertyList DLL", 0);
	} else if (fdwReason == DLL_PROCESS_DETACH) {
		UnregisterClass(TEXT("PropertyList"), hinstDLL);
	}
	return TRUE;
}

__declspec(dllexport) int InitPropertyListDll(HINSTANCE hInstance)
{
	proplist_class.hInstance = hInstance;
	//proplist_class.hbrBackground = GetStockObject(WHITE_BRUSH);
	if (!RegisterClassEx(&proplist_class)) {
		MessageBox(NULL, TEXT("Couldn't register class \"PropertyList\"!"), TEXT("Error"), 0);
		return 0;
	}
	hThisInstance = hInstance;
	return 1;
}

__declspec(dllexport) TCHAR *GetPropListClassname()
{
	return proplist_class.lpszClassName;
}

int UpdateScrollInfo(OBJECT *obj)
{
	SCROLLINFO si;

	si.cbSize = sizeof(si);
	si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
	si.nPos = 0;
	si.nMin = 0;
	si.nMax = obj->item_count - 1;
	si.nPage = obj->height / (obj->item_height - 1);
	SetScrollInfo(obj->hwnd, SB_VERT, &si, 1);
	return 1;
}

LRESULT CALLBACK MainWindowProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	OBJECT *obj;
	CREATESTRUCT *cs;
	ITEM newitem;
	DLIST_NODE(ITEM) *node;
	TCHAR buf[64], *p, **pp;
	int i;
	static int yPos;
	SCROLLINFO si;
	RECT rect;
	//COMBOBOXINFO cbInfo;

	obj = GetProp(hwnd, TEXT("ctrl_info"));

  switch (msg) {
		case WM_CREATE:
			obj = malloc(sizeof(OBJECT));
			if (obj == NULL) {
				return -1;
			}
			memset(obj, 0, sizeof(OBJECT));
			SetProp(hwnd, TEXT("ctrl_info"), obj);
			cs = (CREATESTRUCT *)lParam;
			obj->item_lst.elem_size = sizeof(ITEM);
			obj->x = cs->x;
			obj->y = cs->y;
			obj->width = cs->cx;
			obj->height = cs->cy;
			obj->hwnd = hwnd;
			if (cs->lpszName != NULL) {
				obj->title = malloc((_tcslen(cs->lpszName) + 1)  * sizeof(TCHAR));
				_tcscpy(obj->title, cs->lpszName);
			}
			obj->style = cs->style;
			obj->exstyle = cs->dwExStyle;
			obj->hParent = cs->hwndParent;
			obj->cur_item = NULL;
			return 0;
		case WM_NCDESTROY:
			RemoveProp(hwnd, TEXT("ctrl_info"));
			FreeObject(obj);
			break;
		case WM_PAINT:
			PaintMainWnd(hwnd, obj, BeginPaint(hwnd, &ps));
			EndPaint (hwnd, &ps);
			return 0;
		case WM_ERASEBKGND:
			return 1;
		case WM_SIZE:
			Resize(obj, LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_MOVE:
			obj->x = LOWORD(lParam);
			obj->y = HIWORD(lParam);
			return 0;
		case WM_SETFONT:
			node = obj->item_lst.first;
			while (node != NULL) {
				SendMessage(node->elem.hLeft, WM_SETFONT, wParam, 0);
				SendMessage(node->elem.hRight, WM_SETFONT, wParam, 0);
				node = node->next;
			}
			obj->hFont = (HFONT)wParam;
			return 1;
		//case WM_SETFOCUS:
			//SetFocus(obj->hParent);
			//return 0;
		case WM_COMMAND:
			//Govnokod
			if (HIWORD(wParam) != CBN_SELCHANGE) break;
			if ((node = SearchItemHWND(obj, (HWND)lParam)) != NULL) {
				switch (node->elem.type) {
					case PL_LIST:
					case PL_BOOL:
						switch (HIWORD(wParam)) {
							case CBN_SELCHANGE:
								node->elem.prev_val.i = node->elem.val.i;
								node->elem.val.i = SendMessage(node->elem.hRight, CB_GETCURSEL, 0, 0);
								if (node->elem.type == PL_BOOL) {
									node->elem.val.i ^= 1;
									node->elem.val.i &= 1;
								}
								if (SendMessage(obj->hParent, WM_COMMAND, MAKEWPARAM(node->elem.id, PLN_ITEMCHANGE), (LPARAM)obj->hwnd) == 0) {
									node->elem.val.i = node->elem.prev_val.i;
									if (node->elem.type == PL_BOOL) {
										node->elem.prev_val.i ^= 1;
										node->elem.prev_val.i &= 1;
									}
									SendMessage(node->elem.hRight, CB_SETCURSEL, node->elem.prev_val.i, 0);
									UpdateWindow(node->elem.hRight);
								}
								break;
						}
						break;
				}
			}
			break;
		case WM_CTLCOLORSTATIC:
			//SetBkMode((HDC)wParam, TRANSPARENT);
			return (long)GetStockObject(WHITE_BRUSH);
		case WM_VSCROLL:
			si.cbSize = sizeof (si);
			si.fMask  = SIF_ALL;
			GetScrollInfo (hwnd, SB_VERT, &si);

			yPos = si.nPos;
			switch (LOWORD (wParam)) {
				case SB_TOP:
					si.nPos = si.nMin;
					break;    
					// User clicked the END keyboard key.
				case SB_BOTTOM:
					si.nPos = si.nMax;
					break; 
					// User clicked the top arrow.
				case SB_LINEUP:
					si.nPos -= 1;
					break;
					// User clicked the bottom arrow.
				case SB_LINEDOWN:
					si.nPos += 1;
					break;
					// User clicked the scroll bar shaft above the scroll box.
				case SB_PAGEUP:
					si.nPos -= si.nPage;
					break; 
					// User clicked the scroll bar shaft below the scroll box.
				case SB_PAGEDOWN:
					si.nPos += si.nPage;
					break;
							// User dragged the scroll box.
				case SB_THUMBTRACK:
					si.nPos = si.nTrackPos;
					break;
		  }

		  // Set the position and then retrieve it.  Due to adjustments
		  // by Windows it may not be the same as the value set.
		  si.fMask = SIF_POS;
		  SetScrollInfo (hwnd, SB_VERT, &si, TRUE);
		  GetScrollInfo (hwnd, SB_VERT, &si);

		  // If the position has changed, scroll window and update it.
		  if (si.nPos != yPos)
		  {                    
			  ScrollWindow(hwnd, 0, (obj->item_height - 1) * (yPos - si.nPos), NULL, NULL);
			  UpdateWindow (hwnd);
		  }

			return 0;
		case PL_ADDITEM:
			memset(&newitem, 0, sizeof(newitem));
			//newitem.val_list.elem_size = sizeof(TCHAR *);
			newitem.type = ((PL_ITEMINFO *)wParam)->type;
			newitem.id = ((PL_ITEMINFO *)wParam)->id;
			switch(newitem.type) {
				case PL_TEXT:
					if (((PL_ITEMINFO *)wParam)->val.s) {
						newitem.val.s = malloc((_tcslen(((PL_ITEMINFO *)wParam)->val.s) + 1) * sizeof(TCHAR));
						_tcscpy(newitem.val.s, ((PL_ITEMINFO *)wParam)->val.s);
					}
					break;
				case PL_INT:
				case PL_BOOL:
					newitem.val.i = ((PL_ITEMINFO *)wParam)->val.i;
					break;
				case PL_LIST:
					newitem.val.h = ((PL_ITEMINFO *)wParam)->val.h;
					break;
				default:
					return 0;
			}
			newitem.hLeft = CreateWindowEx(0, WC_STATIC, ((PL_ITEMINFO *)wParam)->name, WS_CLIPSIBLINGS | WS_CHILD | WS_VISIBLE | WS_BORDER, 0, obj->item_count * (obj->item_height - 1) - 1, 0, 0, /*obj->width / 2, obj->item_height, */hwnd, NULL, hThisInstance, NULL);
			if (newitem.hLeft == NULL) return 0;
			SendMessage(newitem.hLeft, WM_SETFONT, (WPARAM)obj->hFont, 0);
			
			switch(newitem.type) {
				case PL_INT:
					_stprintf(buf, TEXT("%d"), newitem.val.i);
				case PL_TEXT:
					newitem.hRight = CreateWindowEx(0, WC_EDIT, (newitem.type == PL_INT) ? buf : newitem.val.s, ES_AUTOHSCROLL | WS_CLIPSIBLINGS | WS_CHILD | WS_VISIBLE | WS_BORDER, 0, 0, 0, 0, /*obj->width / 2 - 1, obj->item_count * (obj->item_height - 1) - 1, obj->width / 2, obj->item_height, */hwnd, NULL, hThisInstance, NULL);
					DefEditProc = (WNDPROC)GetWindowLongPtr(newitem.hRight, GWLP_WNDPROC);
					SetWindowLongPtr(newitem.hRight, GWLP_WNDPROC, (LONG)MyEditProc);
					break;
				case PL_BOOL:
				case PL_LIST:
					newitem.hRight = CreateWindowEx(0, WC_COMBOBOX, NULL, WS_CLIPSIBLINGS | WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_NOINTEGRALHEIGHT, 0, 0, 0, 0,/*obj->width / 2 - 1, obj->item_count * (obj->item_height - 1) - 1, obj->width / 2, obj->item_height * 4*/ hwnd, NULL, hThisInstance, NULL);
					//GetComboBoxInfo(newitem.hRight, &cbInfo);
					//SendMessage(cbInfo.hwndItem, EM_SETREADONLY, 1, 0);
					//SendMessage(GetDlgItem(newitem.hRight, 1001), EM_SETREADONLY, 1, 0);
					if (newitem.type == PL_BOOL) {
						SendMessage(newitem.hRight, CB_ADDSTRING, 0, (LPARAM)_T("True"));
						SendMessage(newitem.hRight, CB_ADDSTRING, 0, (LPARAM)_T("False"));
						SendMessage(newitem.hRight, CB_SETCURSEL, (newitem.val.i == 0) ? 1 : 0, 0);
					} else {
						pp = newitem.val.h;
						if (!pp) break;
						while (*pp != NULL) {
							SendMessage(newitem.hRight, CB_ADDSTRING, 0, (LPARAM)*pp);
							pp++;
						}
						//SendMessage(newitem.hRight, CB_SETMINVISIBLE, 2, 0);
						SendMessage(newitem.hRight, CB_SETCURSEL, LOWORD(lParam), 0);
					}
					break;
			}
			if (newitem.hRight == NULL) return 0;
			SendMessage(newitem.hRight, WM_SETFONT, (WPARAM)obj->hFont, 0);

			if (((PL_ITEMINFO *)wParam)->name) {
				newitem.name = malloc((_tcslen(((PL_ITEMINFO *)wParam)->name) + 1) * sizeof(TCHAR));
				_tcscpy(newitem.name, ((PL_ITEMINFO *)wParam)->name);
			}

			DListAdd(&obj->item_lst, (void *)-1, &newitem);
			obj->item_count++;
			return 1;


		case PL_DELETEITEM:
			DeleteItem(obj, wParam);
			return 1;
		case PL_SETITEMVALUE:
			node = SearchItem(obj, ((PL_ITEMINFO *)wParam)->id);
			if (node != NULL) {
				switch(node->elem.type) {
					case PL_INT:
						_stprintf(buf, TEXT("%d"), ((PL_ITEMINFO *)wParam)->val.i);
						node->elem.val.i = ((PL_ITEMINFO *)wParam)->val.i;
						SetWindowText(node->elem.hRight, buf);
						break;
					case PL_TEXT:
						if (((PL_ITEMINFO *)wParam)->val.s == NULL) break;
						free(node->elem.val.s);
						free(node->elem.prev_val.s);
						node->elem.val.s = malloc((_tcslen(((PL_ITEMINFO *)wParam)->val.s) + 1) * sizeof(TCHAR));
						_tcscpy(node->elem.val.s, ((PL_ITEMINFO *)wParam)->val.s);
						SetWindowText(node->elem.hRight, node->elem.val.s);
						break;
					case PL_BOOL:
					case PL_LIST:
						i = ((PL_ITEMINFO *)wParam)->val.i;
						if (node->elem.type == PL_BOOL) i = i ^ 1 & 1; //owned
						SendMessage(node->elem.hRight, CB_SETCURSEL, i, 0);
						break;
				}
			}
			return 1;
		case PL_GETITEMVALUE:
			if ((node = SearchItem(obj, (int)wParam)) != NULL) {
				switch(node->elem.type) {
					case PL_BOOL:
					case PL_LIST:
					case PL_INT:
						((PL_VALUE *)lParam)->i = node->elem.val.i;
					case PL_TEXT:
						((PL_VALUE *)lParam)->s = node->elem.val.s;
				}
				return 1;
			} else {
				return 0;
			}
			break;
		case PL_CLEAN:
			CleanList(obj);
			UpdateScrollInfo(obj);
			return 1;
		case PL_SETITEMHEIGHT:
			obj->item_height = (int)wParam;
			return 1;
		//case PL_UPDATESCROLL:
			//break;
		case PL_STARTFILL:
			obj->filling = 1;
			CleanList(obj);
			//UpdateScrollInfo(obj);
			break;
		case PL_ENDFILL:
			obj->filling = 0;
			GetClientRect(obj->hwnd, &rect);
			Resize(obj, rect.right - rect.left, rect.bottom - rect.top);
			break;
		case PL_PROCESSCHANGEDPROP:
			return ProcessChangedItem(obj, obj->cur_item);
  }

  return DefWindowProc(hwnd, msg, wParam, lParam);
}

int ProcessChangedItem(OBJECT *obj, ITEM *item)
{
	TCHAR buf[64], *p;
	int i;

	if (item == NULL) return 0;

	switch (item->type) {
		case PL_INT:
			item->prev_val.i = item->val.i;
			if (SendMessage(item->hRight, WM_GETTEXTLENGTH, 0, 0) > 20) break;
			SendMessage(item->hRight, WM_GETTEXT, 21, (LPARAM)buf); 
			item->val.i = _ttoi(buf);
			if (SendMessage(obj->hParent, WM_COMMAND, MAKEWPARAM(item->id, PLN_ITEMCHANGE), (LPARAM)obj->hwnd) == 0) {
				item->val.i = item->prev_val.i;
				_stprintf(buf, TEXT("%d"), item->val.i);
				SetWindowText(item->hRight, buf);
			}
			break;
		case PL_TEXT:
			item->prev_val.s = item->val.s;
			i = SendMessage(item->hRight, WM_GETTEXTLENGTH, 0, 0) + 1;
			item->val.s = malloc(i * sizeof(TCHAR));
			SendMessage(item->hRight, WM_GETTEXT, i, (LPARAM)item->val.s); 
			if (SendMessage(obj->hParent, WM_COMMAND, MAKEWPARAM(item->id, PLN_ITEMCHANGE), (LPARAM)obj->hwnd) == 0) {
				free(item->val.s);
				item->val.s = item->prev_val.s;
				item->prev_val.s = NULL;
				SetWindowText(item->hRight, item->val.s);
			} else {
				free(item->prev_val.s);
			}
			break;
	}
	return 1;
}

LRESULT CALLBACK MyEditProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	OBJECT *obj;
	DLIST_NODE(ITEM) *node;

	obj = GetProp(GetParent(hwnd), TEXT("ctrl_info"));

	switch (msg) {
		case WM_SETFOCUS:
			obj->cur_item = &(node = SearchItemHWND(obj, hwnd))->elem;
			break;
		case WM_KEYDOWN:
		case WM_KILLFOCUS:
			if (msg == WM_KEYDOWN && wParam != VK_RETURN) break;
			if (msg == WM_KILLFOCUS && SearchItemHWND(obj, (HWND)wParam) == NULL) break;
			if ((node = SearchItemHWND(obj, hwnd)) != NULL) {
				ProcessChangedItem(obj, &node->elem);
				if (msg == WM_KEYDOWN && wParam == VK_RETURN) return 0;
			}
	}
	return DefEditProc(hwnd, msg, wParam, lParam);
}

void Resize(OBJECT *obj, int w, int h)
{
	DLIST_NODE(ITEM) *l_node;
	RECT rect;
	int height, y;
	COMBOBOXINFO info = { sizeof(COMBOBOXINFO) };
	
	if (obj->filling) return;

	obj->width = w;
	obj->height = h;

	//UpdateScrollInfo(obj);

	l_node = obj->item_lst.first;
	y = -1;
	while (l_node != NULL) {
		SetWindowPos(l_node->elem.hLeft, NULL, 0, y, w / 2, obj->item_height, /*SWP_NOMOVE | */SWP_NOZORDER);
		y += obj->item_height - 1;
		GetWindowRect(l_node->elem.hLeft, &rect);
		MapWindowPoints(HWND_DESKTOP, obj->hwnd, (POINT *)&rect, 2);
		height = obj->item_height;
		if (l_node->elem.type == PL_BOOL || l_node->elem.type == PL_LIST) {
			height *= (SendMessage(l_node->elem.hRight, CB_GETCOUNT, 0, 0) + 1);
		}
		SetWindowPos(l_node->elem.hRight, NULL, w / 2 - 1, rect.top, w / 2 + 2, height, SWP_NOZORDER | SWP_NOREDRAW);
		if (l_node->elem.type == PL_BOOL || l_node->elem.type == PL_LIST) {
			GetComboBoxInfo(l_node->elem.hRight, &info);
			SendMessage(l_node->elem.hRight, CB_SETITEMHEIGHT, -1, obj->item_height - info.rcItem.top * 2);
		}
		l_node = l_node->next;
	}

	UpdateScrollInfo(obj);
	return;
}


void PaintMainWnd(HWND hwnd, OBJECT *obj, HDC hDC)
{
	HPEN hOldPen;
	HBRUSH hOldBrush;
	RECT rect;

	hOldBrush = SelectObject(hDC,GetStockObject(WHITE_BRUSH));
	hOldPen = SelectObject(hDC,GetStockObject(WHITE_PEN));
	
	GetClientRect(hwnd, &rect);
	Rectangle(hDC, 0, 0, rect.right, rect.bottom);

	SelectObject(hDC, hOldPen);
	SelectObject(hDC, hOldBrush);
	return;
}

void *SearchItem(OBJECT *obj, int id)
{
	DLIST_NODE(ITEM) *cur_node;

	cur_node = obj->item_lst.first;
	while (cur_node != NULL) {
		if (cur_node->elem.id == id) break;
		cur_node = cur_node->next;
	}
	return cur_node;
}

void *SearchItemHWND(OBJECT *obj, HWND hRight)
{
	DLIST_NODE(ITEM) *cur_node;

	cur_node = obj->item_lst.first;
	while (cur_node != NULL) {
		if (cur_node->elem.hRight == hRight) break;
		cur_node = cur_node->next;
	}
	return cur_node;
}

void FreeObject(OBJECT *obj)
{
	CleanList(obj);
	if (obj->title) free(obj->title);
	free(obj);
	return;
}

void CleanList(OBJECT *obj)
{
	DLIST_NODE(ITEM) *cur_node;

	(void *)cur_node =  obj->item_lst.first;
	while (cur_node != NULL) {
		if (cur_node->elem.type == PL_TEXT) {
			free(cur_node->elem.val.s);
			free(cur_node->elem.prev_val.s);
		}
		free(cur_node->elem.name);
		DestroyWindow(cur_node->elem.hLeft);
		DestroyWindow(cur_node->elem.hRight);
		cur_node = cur_node->next;
	}

	DListFree(&obj->item_lst);
	obj->item_count = 0;
	obj->cur_item = NULL;
	return;
}

void DeleteItem(OBJECT *obj, int id)
{
	DLIST_NODE(ITEM) *cur_node;

	cur_node = SearchItem(obj, id);
	if (cur_node != NULL) {
		if (cur_node->elem.type == PL_TEXT) {
			if (cur_node->elem.val.s) free(cur_node->elem.val.s);
		}
		if (cur_node->elem.name) free(cur_node->elem.name);
		DestroyWindow(cur_node->elem.hLeft);
		DestroyWindow(cur_node->elem.hRight);
		DListRemove(&obj->item_lst, cur_node);
		obj->item_count--;
	}
}