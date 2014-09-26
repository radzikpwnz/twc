#ifndef TWC_DESIGN_H
#define TWC_DESIGN_H

#include <windows.h>
#include <CommCtrl.h>
#include <tchar.h>


#define TWC_CHECKERS_ENABLED

/* Checker */
#ifdef TWC_CHECKERS_ENABLED
#define TWC_CHECKIT(x) { if ( !(x) ) twc_Fatal( T(__FILE__), __LINE__); }
#else
#define TWC_CHECKIT(x)
#endif

/* Boolean type */
typedef BOOL TWC_BOOL;

/* True/false constants */
#define TWC_FALSE 0
#define TWC_TRUE  1

/* Error/success constants */
#define TWC_ERROR   0
#define TWC_SUCCESS 1

/* Useful macroses */
#define sqr( x) ((x) * (x))
#define MIN( x, y) (((x)>(y))?(y):(x))
#define MAX( x, y) (((x)>(y))?(x):(y))
#define T( x) TEXT( x)


#include "mylist.h"
#include "tools.h"
#include "ctrl_info.h"
#include "properties.h"
#include "object.h"
#include "control.h"
#include "window.h"


/**
 * Global variables
 */
extern HWND hMainWnd;
extern HWND hPropList;
extern RT_OBJECT *current_object;
extern int grid_size;
extern int new_control;


/**
 * Terminate program and show error message.
 */
void twc_Fatal( TCHAR *file, int line);

#endif
