#include <windows.h>

#include "twc_design.h"

#include "project.h"


/* Current project info */
TWCD_PROJECT *cur_project;


TWCD_PROJECT *NewProject()
{
    TWCD_PROJECT *project;
    RT_OBJECT *root_obj;

    project = malloc( sizeof(TWCD_PROJECT));

    root_obj = (RT_OBJECT *)calloc( 1, sizeof(RT_OBJECT));
    root_obj->id = CTRL_ID_ROOT;
    DListInit( &root_obj->child_list, sizeof(PRT_OBJECT));

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
 *
 * TODO: Make root object!!!!
 */
DLIST_PRT_OBJECT *GetParentChildList( RT_OBJECT *obj) /* object */
{
    TWC_CHECKIT( !IsObjectRoot( obj) );
    return &obj->parent->child_list;
}

DLIST_PRT_OBJECT *GetProjectChildList( TWCD_PROJECT *project) /* project */
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
        DestroyObjectWindow( node->elem, TWC_TRUE);
    OBJ_LIST_ITERATE_END();

    FreeProject( cur_project);

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
    RT_OBJECT *obj;

    OBJ_LIST_ITERATE_BEGIN( GetProjectChildList( cur_project));
        if ( CreateObjectWindow( node->elem, TWC_TRUE) == 0) {
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
