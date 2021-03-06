#include <windows.h>

#include "twc_design.h"

#include "project.h"


/* Current project info */
TWCD_PROJECT *cur_project;


TWCD_PROJECT *NewProject()
{
    TWCD_PROJECT *project;
    TWC_OBJECT *root_obj;

    project = malloc( sizeof(TWCD_PROJECT));

    root_obj = (TWC_OBJECT *)calloc( 1, sizeof(TWC_OBJECT));
    root_obj->id = CTRL_ID_ROOT;
    DListInit( &root_obj->child_list, sizeof(PTWC_OBJECT));

    project->version.maj = 1;
    project->version.min = 0;
    project->root_object = root_obj;
    project->path = NULL;

    return project;
}

//TODO: copy project

int FreeProject( TWCD_PROJECT *project)
{
    FreeObject( project->root_object);
    free( project->path);
    free( project);
    return 1;
}

/**
 * Get parent object child list.
 */
DLIST_PTWC_OBJECT *GetParentChildList( TWC_OBJECT *obj) /* object */
{
    TWC_CHECKIT( !IsObjectRoot( obj) );
    return &obj->parent->child_list;
}

DLIST_PTWC_OBJECT *GetProjectChildList( TWCD_PROJECT *project) /* project */
{
    TWC_CHECKIT( project != NULL);
    TWC_CHECKIT( project->root_object != NULL);

    return &project->root_object->child_list;
}


/**
 * Unload project.
 *
 * All windows are destroyed and objects freed.
 */
void UnloadCurrentProject()
{
    if ( cur_project == NULL ) {
        return;
    }

    SetCurrentObject( NULL);

    OBJ_LIST_ITERATE_BEGIN( GetProjectChildList( cur_project));
        DestroyObjectWindow( NODE()->elem, TWC_TRUE);
    OBJ_LIST_ITERATE_END();

    FreeProject( cur_project);
    cur_project = NULL;

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
    TWC_OBJECT *obj;

    OBJ_LIST_ITERATE_BEGIN( GetProjectChildList( cur_project));
        if ( CreateObjectWindow( NODE()->elem, TWC_TRUE) == 0) {
            if ( MessageBox( hMainWnd, T("Error creating one of windows! Continue loading?"), T("Error"), MB_YESNO) == IDNO ) {
                return 0;
            }
        }
    OBJ_LIST_ITERATE_END();

    obj = GetProjectChildList( cur_project)->first->elem;
    if ( obj != NULL ) {
        TWC_CHECKIT( IsObjectWindow( obj) );
        SetCurrentObject( obj);
        SetFocus( obj->hwnd);
    }

    return 1;
}
