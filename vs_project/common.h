#ifndef TWCD_COMMON_H
#define TWCD_COMMON_H

#include <windows.h>
#include <CommCtrl.h>
#include <tchar.h>

typedef BOOL TWC_BOOL;

#include "mylist.h"

#include "object.h"
#include "ctrl_info.h"

#include "project.h"
#include "control.h"

#include "tools.h"
#include "stuff.h"


#define TWC_CHECKERS_ENABLED

/* Checker */
#ifdef TWC_CHECKERS_ENABLED
#define TWC_CHECKIT(x) { if ( !(x) ) twc_Fatal( T(__FILE__), __LINE__); }
#else
#define TWC_CHECKIT(x)
#endif

/* True/false constants */
#define TWC_FALSE 0
#define TWC_TRUE  1

/* Error/success constants */
#define TWC_ERROR   0
#define TWC_SUCCESS 1

/* Useful macroses */
#define sqr(x) ((x) * (x))
#define MIN(x,y) (((x)>(y))?(y):(x))
#define MAX(x,y) (((x)>(y))?(x):(y))
#define T(x) TEXT(x)

/* TODO: remove */
#define MDICLIENT_CLASS T("MDICLIENT")

/* Control minimal height and width */
#define CONTROL_MIN_WIDTH 10
#define CONTROL_MIN_HEIGHT 10

/**
 * Global variables
 */
extern HWND hMainWnd;
extern HWND hPropList;
extern RT_OBJECT *current_object;
extern int grid_size;
extern int new_control;


int SetString(TCHAR **str, TCHAR *new_str, int copy);

void SetStatusText(TCHAR *str);

#define free(ptr) my_free(&(ptr))
void my_free(void **p);

TCHAR *_mytcscpy(TCHAR *dest, const TCHAR *source);

const TCHAR *GetProgramVersion();

void twc_Fatal( TCHAR *file, int line);

#endif
