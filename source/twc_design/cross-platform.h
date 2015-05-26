#ifndef CROSS_PLATFORM_H
#define CROSS_PLATFORM_H

#include "twc_design.h"


typedef struct tagWND_DESCR {
#if WINDOWS
    TCHAR *classname;               /* class name */
    HWND hwnd;                      /* object and static HWND */ 
    HWND static_hwnd;           
    WNDPROC orig_wndproc;           /* object and static original window procedures */
    WNDPROC static_orig_wndproc;
    DWORD style;                    /* style and extended style */
    DWORD exstyle;
#elif LINUX

#endif
} WND_DESCR;


/**
 * Main object info structure
 */
struct _tagTWC_OBJECT {
    UINT ctrl_id;                   /* control ID */
    DLIST_PTWC_OBJECT child_list;    /* child list */

    int selected;                   /* if object selected */ 
    int x, y;                       /* position */
    int width, height;              /* size */

    TWC_OBJECT *parent;              /* parent object (NULL for window) */
    PROPERTY *properties;           /* properties array */
    
    WND_DESCR wnd_descr;

    TCHAR *name;                    /* name */
    TCHAR *title;                   /* title or text */
    UINT flags;                     /* flags */
};






#endif