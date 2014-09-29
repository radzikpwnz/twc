#include <windows.h>

#include "twc_design.h"

#include "project.h"


/* Current project info */
TWCD_PROJECT *cur_project;


TWCD_PROJECT *NewProject()
{
    TWCD_PROJECT *project;
    RT_OBJECT *root_obj;

    project = malloc( sizeof(TWCD_PROJECT))

    root_obj = (RT_OBJECT *)calloc( 1, sizeof(RT_OBJECT));
    root_obj->id = CTRL_ID_ROOT;
    DListInit( root_obj->child_list, sizeof(PRT_OBJECT));

    project->root_object = root_obj;

    return project;
}

//TODO: copy project

int FreeProject( TWCD_PROJECT *project)
{
    FreeObject( project->root_object);
    free( project->path);
    return 1;
}

/**
 * Get parent object child list.
 *
 * TODO: Make root object!!!!
 */
DLIST_PRT_OBJECT *GetParentChildList( RT_OBJECT *obj) /* object */
{
    TWC_CHECKIT( obj->id != CTRL_ID_ROOT );
    return &obj->parent->child_list;
}

/**
 * Unload current project.
 *
 * All windows are destroyed and objects freed.
 */
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

/**
 * Load current project.
 *
 * Creates windows for all objects in project, with childs.
 */
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
