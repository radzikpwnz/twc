#ifndef LIBTWC_H
#define LIBTWC_H

#include <windows.h>
#include <CommCtrl.h>
#include <tchar.h>

/* Boolean type */
typedef BOOL TWC_BOOL;

/* True/false constants */
#define TWC_FALSE 0
#define TWC_TRUE  1

/* Error/success constants */
#define TWC_ERROR   0
#define TWC_SUCCESS 1

#include "../tools/mylist.h"
#include "../tools/tools.h"

#ifdef BUILD_DLL

#define EXPORT __declspec(dllexport)

#define TWC_CHECKIT(x)
#define TWC_CHECKDO(x) (x)

/* Useful macroses */
#define sqr( x) ((x) * (x))
#define MIN( x, y) (((x)>(y))?(y):(x))
#define MAX( x, y) (((x)>(y))?(x):(y))
#define T( x) TEXT( x)

#else

#define EXPORT

#include "object.h"
#include "properties.h"
#include "ctrl_info.h"

#endif

#endif