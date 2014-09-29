#ifndef TWC_DESIGN_H
#define TWC_DESIGN_H

#define WINDOWS ( defined( _WIN32) || defined( WIN32) )
#define LINUX ( defined( linux) || defined( __linux__) )

#if WINDOWS
    #include <windows.h>
    #include <CommCtrl.h>
    #include <tchar.h>
#endif

#if LINUX
    #include <unistd.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
#endif

#define TWC_CHECKERS_ENABLED

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


#endif