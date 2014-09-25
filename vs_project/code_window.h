#ifndef CODE_WINDOW_H
#define CODE_WINDOW_H

#include <windows.h>

typedef struct tagCODE_WINDOW CODE_WINDOW;

#include "common.h"

struct tagCODE_WINDOW {
	HWND hwnd;
	TCHAR *code;
	RT_OBJECT *window;
	HWND edit_hwnd;
};


LRESULT CALLBACK CodeWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
int CreateCodeWindow(RT_WINDOW *window, TCHAR *code);

#endif