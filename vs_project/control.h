#ifndef CONTROL_H
#define CONTROL_H

#include "common.h"

RT_OBJECT *CreateControlFromToolbox(RT_WINDOW *cur_wnd, int x, int y);
LRESULT CALLBACK ControlWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif