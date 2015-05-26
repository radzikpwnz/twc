#include <windows.h>
#include <Richedit.h>

#include "twc_design.h"

#include "interface.h"
//#include "..\property_list\proplist.h"
//#include "proplst.h"
#include "mainwnd.h"
#include "code_window.h"

typedef struct tagCONTROL_EVENT_INFO CONTROL_EVENT_INFO;

enum tagPARAM_FORMAT {
	PF_DWORD_DWORD,
	PF_DWORD_NULL,
	PF_NULL_DWORD,
	PF_WORD_WORD_NULL,
	PF_NULL_WORD_WORD,
	PF_WORD_WORD_DWORD,
	PF_DWORD_WORD_WORD,
	PF_DWORD_WORD_NULL,
	PF_WORD_NULL_DWORD,
	PF_NULL_NULL
};

struct tagCONTROL_EVENT_INFO {
	TCHAR *name;
	UINT message;
	TCHAR *notif_str;
	UINT param_format;
	struct tagPARAM_INFO {
		TCHAR *type;
		TCHAR *name;
	} param[4];
};

CONTROL_EVENT_INFO tmp = {
	T("Click"),
	0,
	NULL,
	0,
	{{T("char *"), T("param1")}, {T("UINT"), T("param2")}}
};

int GenerateEventHandler(RT_OBJECT *obj, CONTROL_EVENT_INFO *cei, TCHAR **pbuf) {
	UINT len, type_len;
	TCHAR *p;
	int i;

	len = _tcslen(obj->name) + 1 + ((obj->id == CTRL_ID_WINDOW) ? 0 : (_tcslen(obj->parent->name) + 1)) + _tcslen(cei->name); //handler_name
	i = 0;
	while (cei->param[i].name != NULL) {
		type_len = _tcslen(cei->param[i].type);
		len += _tcslen(cei->param[i].name) + type_len + ((cei->param[i].type[type_len - 1] == T('*')) ? 2 : 3); //param type + param name + 1 if type is not pointer (TYPE_name) + ', '
		i++;
	}
	len += 1 - 1 + 2 + 2 + 3; //'1 space ()_' - '1 space in (..._)' + '()' + '{}' + '3x \n'

	*pbuf = malloc((len + 1) * sizeof(TCHAR));
	p = *pbuf;
	
	//handler name
	if (obj->id != CTRL_ID_WINDOW) {
		p = _mytcscpy(p, obj->parent->name);
		*p++ = T('_');
	}
	p = _mytcscpy(p, obj->name);
	*p++ = T('_');
	p = _mytcscpy(p, cei->name);

	*p++ = T(' ');
	*p++ = T('(');

	//params
	i = 0;
	while (cei->param[i].name != NULL) {
		p = _mytcscpy(p, cei->param[i].type);
		if (*(p - 1) != T('*')) {
			*p++ = T(' ');
		}
		p = _mytcscpy(p, cei->param[i].name);
		*p++ = T(',');
		*p++ = T(' ');
		i++;
	}

	//other
	p = _mytcscpy(p - 2, T(") {\n\n}\n"));

	return 1;
}


LRESULT CALLBACK CodeWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	CODE_WINDOW *cur_wnd;
	UINT len;
	CHARFORMAT cf;

	cur_wnd = (CODE_WINDOW *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch (msg) {
		case WM_NCCREATE:
			if (((CREATESTRUCT *)lParam)->lpCreateParams && (LONG )(  ((MDICREATESTRUCT *)(((CREATESTRUCT *)lParam)->lpCreateParams))->lParam )) {
				SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG )(  ((MDICREATESTRUCT *)(((CREATESTRUCT *)lParam)->lpCreateParams))->lParam ) ); //fuck yeah
			}
			cur_wnd = (CODE_WINDOW *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
			cur_wnd->hwnd = hwnd;
			break;
		case WM_CREATE:
			cur_wnd->edit_hwnd = CreateWindowEx(0, /*((msft_mod == NULL) ? RICHEDIT_CLASS : */ MSFTEDIT_CLASS, NULL, /*ES_WANTRETURN | */ WS_HSCROLL | WS_VSCROLL | WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL, 0, 0, 0, 0, cur_wnd->hwnd, NULL, GetModuleHandle(NULL), NULL);
			memset(&cf, 0, sizeof(cf));//ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_WANTRETURN | WS_BORDER | WS_VSCROLL | WS_HSCROLL
			cf.dwMask = CFM_CHARSET | CFM_FACE | CFM_SIZE | CFM_OFFSET | CFM_COLOR;
			cf.dwMask ^= CFM_ITALIC ^ CFM_BOLD ^ CFM_STRIKEOUT ^ CFM_UNDERLINE;
			cf.dwEffects = 0;
			cf.yHeight = 200; //10pts * 20 twips/point = 200 twips
			cf.bCharSet = DEFAULT_CHARSET;
			cf.bPitchAndFamily = FIXED_PITCH | FF_MODERN;
			cf.yOffset = 0;
			cf.crTextColor = RGB(0,0,0);
			_tcscpy(cf.szFaceName, T("Courier New"));
			cf.cbSize = sizeof(cf);
			SendMessage(cur_wnd->edit_hwnd, EM_SETCHARFORMAT, 0, (LPARAM)&cf);
			if (cur_wnd->code) SetWindowText(cur_wnd->edit_hwnd, cur_wnd->code);
			SetFocus(cur_wnd->edit_hwnd);
			break;
		case WM_DESTROY:
			if (cur_wnd->code != NULL) {
				free(cur_wnd->code);
			}
			len = GetWindowTextLength(cur_wnd->edit_hwnd);
			cur_wnd->code = malloc((len + 1) * sizeof(TCHAR));
			GetWindowText(cur_wnd->edit_hwnd, cur_wnd->code, len + 1);
			cur_wnd->hwnd = cur_wnd->edit_hwnd = NULL;
			break;
		case WM_ERASEBKGND:
			return 1;
		case WM_SIZE:
			SetWindowPos(cur_wnd->edit_hwnd, NULL, 0, 0, LOWORD(lParam), HIWORD(lParam), SWP_NOZORDER);
			break;
	}

	return DefMDIChildProc(hwnd, msg, wParam, lParam);
}

int CreateCodeWindow(RT_WINDOW *window, TCHAR *code)
{
	TCHAR *title;

	if (window->code_window == NULL) {
		window->code_window = (CODE_WINDOW *)malloc(sizeof(CODE_WINDOW));
		memset(window->code_window, 0, sizeof(CODE_WINDOW));
		window->code_window->window = window;
	}

	if (window->code_window->hwnd == NULL) {
		title = (TCHAR *)malloc((_tcslen(window->name) + 10) * sizeof(TCHAR));
		_mytcscpy(_mytcscpy(title, window->name), T(" - code"));
		CreateWindowEx(WS_EX_MDICHILD, T("CodeWnd"), title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, MDIClient.hwnd, NULL, GetModuleHandle(NULL), window->code_window);
		free(title);
		if (window->code_window->hwnd == NULL) {
			return 0;
		}
	}

	//GenerateEventHandler(window, &tmp, &title);

	return 1;
}