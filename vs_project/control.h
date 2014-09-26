#ifndef CONTROL_H
#define CONTROL_H

#include "twc_design.h"

/**
 * Create control from toolbox.
 */
RT_OBJECT *CreateControlFromToolbox(RT_WINDOW *cur_wnd, int x, int y);

/**
 * Control window procedure.
 */
LRESULT CALLBACK ControlWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif