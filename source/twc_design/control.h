#ifndef CONTROL_H
#define CONTROL_H

#include "twc_design.h"


/* Control minimal height and width */
#define CONTROL_MIN_WIDTH 10
#define CONTROL_MIN_HEIGHT 10


/**
 * Create control from toolbox.
 */
TWC_OBJECT *CreateControlFromToolbox( TWC_OBJECT *cur_wnd, int x, int y);


#if defined( _WIN32) || defined( WIN32)
/**
 * Control window procedure.
 */
LRESULT CALLBACK ControlWndProc( TWC_OBJECT *obj, UINT msg, WPARAM wParam, LPARAM lParam);

#elif defined( linux)

#endif


#endif