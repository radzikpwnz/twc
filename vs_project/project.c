#include <windows.h>

#include "common.h"

#include "interface.h"

#include "project.h"

TWCD_PROJECT cur_project = {{0, 0}, {NULL, NULL, sizeof(RT_CONTROL *)}, NULL};

void UnloadCurrentProject()
{
    SetCurrentObject( NULL);

    OBJ_LIST_ITERATE_BEGIN( &cur_project.obj_list );
        DestroyObjectWindow( node->elem, TWC_TRUE);
    OBJ_LIST_ITERATE_END();

    if ( cur_project.path ) {
        free(cur_project.path);
        cur_project.path = NULL;
    }

    SetStatusText( NULL);
    return;
}

int LoadCurrentProject()
{
    OBJ_LIST_ITERATE_BEGIN( &cur_project.obj_list);
        if ( CreateObjectWindow( node->elem, TWC_TRUE) == 0) {
            if ( MessageBox( hMainWnd, T("Error creating one of windows! Continue loading?"), T("Error"), MB_YESNO) == IDNO ) {
                return 0;
            }
        }
    OBJ_LIST_ITERATE_END();

    if ( cur_project.obj_list.last ) {
        SetCurrentObject( cur_project.obj_list.last->elem );
        SetFocus( cur_project.obj_list.last->elem->hwnd);
    }
    return 1;
}