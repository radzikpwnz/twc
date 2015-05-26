#include "twc_design.h"

#include "cross-platform.h"

WND_DESCR *twc_GetObjectWndDescr( RT_OBJECT *obj);


/**
 * Create object window.
 */
int _CreateObjectWindow( RT_OBJECT *obj,         /* object */
                         TWC_BOOL create_childs) /* TWC_TRUE for create childs */
{
#if WINDOWS
    return CreateObjectWindow( obj, create_childs);
#elif LINUX

#endif
}


int _MoveObjectWindow();
int _SizeObjectWindow();

int CALLBACK _PaintObjectWindow();

