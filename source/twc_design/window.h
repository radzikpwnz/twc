#ifndef WINDOW_H
#define WINDOW_H

#include <windows.h>

#include "object.h"

LRESULT CALLBACK ChildWndProc( TWC_OBJECT *obj, UINT msg, WPARAM wParam, LPARAM lParam);

HWND AddWindow();
int PreviewWindow( TWC_OBJECT *obj, HWND hParent);

#endif