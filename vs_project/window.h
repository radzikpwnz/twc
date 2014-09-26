#ifndef WINDOW_H
#define WINDOW_H

#include <windows.h>

#include "object.h"

LRESULT CALLBACK ChildWndProc( HWND, UINT, WPARAM, LPARAM);

HWND AddWindow();
int PreviewWindow( RT_OBJECT *obj, HWND hParent);

#endif