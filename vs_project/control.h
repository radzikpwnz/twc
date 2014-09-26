#ifndef CONTROL_H
#define CONTROL_H

#include <windows.h>

#include "object.h"

/* Control minimal height and width */
#define CONTROL_MIN_WIDTH 10
#define CONTROL_MIN_HEIGHT 10


/**
 * Create control from toolbox.
 */
RT_OBJECT *CreateControlFromToolbox( RT_OBJECT *cur_wnd, int x, int y);

/**
 * Control window procedure.
 */
LRESULT CALLBACK ControlWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif