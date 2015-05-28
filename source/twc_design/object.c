#include <windows.h>

#include "twc_design.h"

#include "proplst.h"
#include "interface.h"
#include "static.h"
#include "project.h"
#include "clipboard.h"
#include "properties.h"

#include "object.h"


/**
 * Copy object.
 */
TWC_OBJECT *CopyObject( TWC_OBJECT *obj,    /* object to copy */
                        TWC_OBJECT *parent) /* parent of object's copy */
{
    TWC_OBJECT *new_obj;

    new_obj = twc_CopyObject( obj, parent);
    OBJ_CLIENT_DATA( new_obj)->static_orig_wndproc = NULL;
    OBJ_CLIENT_DATA( new_obj)->static_hwnd = NULL;

    OBJ_CLIENT_DATA( new_obj)->selected = 0;
    
    /* Generate object name and make title point to title property value */
    GenerateObjectName( new_obj);
    new_obj->title = twc_GetObjectPropertyVal( new_obj, COMMON_TITLE)->s;

    return new_obj;
}

/**
 * Free object.
 */
int FreeObject( TWC_OBJECT *obj) /* object */
{
    TWC_CHECKIT( obj != current_object );

    RemoveObjectFromList( obj, GetClipboard());
    twc_FreeObject( obj);

    return 1;
}

/**
 * Set default values for new object.
 */
void SetNewObjectDefaultValues( TWC_OBJECT *obj) /* object */
{
    switch ( obj->id ) {
        case CTRL_ID_WINDOW:
            SetObjectPropertyStr( obj, COMMON_TITLE, T("New window"), TWC_TRUE, TWC_FALSE);
            break;
        case CTRL_ID_BUTTON:
            SetObjectPropertyStr( obj, COMMON_TITLE, T("Button"), TWC_TRUE, TWC_FALSE);
            break;
        case CTRL_ID_CHECKBOX:
            SetObjectPropertyStr( obj, COMMON_TITLE, T("Check box"), TWC_TRUE, TWC_FALSE);
            break;
        case CTRL_ID_RADIOBUTTON:
            SetObjectPropertyStr( obj, COMMON_TITLE, T("Radio button"), TWC_TRUE, TWC_FALSE);
            break;
        case CTRL_ID_STATIC:
            SetObjectPropertyStr( obj, COMMON_TITLE, T("Static"), TWC_TRUE, TWC_FALSE);
            break;
        case CTRL_ID_EDIT:
            SetObjectPropertyStr( obj, COMMON_TITLE, T("Edit box"), TWC_TRUE, TWC_FALSE);
            SetObjectPropertyInt( obj, COMMON_BORDER, 1, TWC_TRUE, TWC_FALSE);
            break;
        case CTRL_ID_LISTBOX:
            SetObjectPropertyInt( obj, COMMON_BORDER, 1, TWC_TRUE, TWC_FALSE);
            break;
        case CTRL_ID_GROUPBOX:
            SetObjectPropertyStr( obj, COMMON_TITLE, T("Group box"), TWC_TRUE, TWC_FALSE);
            break;
    }

    return;
}

/**
 * Create static on control.
 */
static int CreateStaticOnControl( TWC_OBJECT *obj) /* object */
{
    HWND hwnd;

    TWC_CHECKIT( IsObjectControl( obj) );

    hwnd = CreateWindowEx( 0, WC_STATIC, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, obj->x, obj->y, obj->width, obj->height,
                           obj->parent->hwnd, NULL, GetModuleHandle(NULL), 0);
    if ( hwnd == NULL ) {
        return 0;
    }
    OBJ_CLIENT_DATA( obj)->static_hwnd = hwnd;

    /* Set OBJECT_INFO property and window procedure */
    SetProp( hwnd, T("OBJECT_INFO"), (HANDLE)obj);
    OBJ_CLIENT_DATA( obj)->static_orig_wndproc = (WNDPROC)SetWindowLongPtr( hwnd, GWLP_WNDPROC, (LONG_PTR)StaticWndProc);

    /* Set z-order */
    SetWindowPos( hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE); //TODO: find exact zorder

    return 1;
}

/**
 * Object initialization after it's been created.
 */
static int InitObjectAfterCreation( TWC_OBJECT *obj) /* object */
{    
    obj->client_wndproc = ( IsObjectWindow( obj) ) ? ChildWndProc : ControlWndProc;

    if ( !IsObjectWindow( obj) ) {
        CreateStaticOnControl( obj);
    }

    return 1;
}

/**
 * Create object window.
 */
int CreateObjectWindow( TWC_OBJECT *obj,        /* object */
                        TWC_BOOL create_childs) /* TWC_TRUE for create childs */
{
    /* Determine parent HWND and MDI style */
    if ( IsObjectRoot( obj->parent) ) {
        obj->parent->hwnd = MDIClient.hwnd;
        obj->exstyle |= WS_EX_MDICHILD;
    }

    twc_CreateObjectWindow( obj, TWC_FALSE, NULL);
    InitObjectAfterCreation( obj);

    if ( create_childs ) {
        OBJ_LIST_ITERATE_BEGIN( &obj->child_list);
            if ( CreateObjectWindow( node->elem, TWC_TRUE) == 0 ) {
                return 0;
            }
        OBJ_LIST_ITERATE_END();
    }

    return 1;
}

/**
 * Destroy object window and free object (if <free_obj> TRUE).
 */
int DestroyObjectWindow( TWC_OBJECT *obj,   /* object */
                         TWC_BOOL free_obj) /* TWC_TRUE for free object */
{
    TWC_CHECKIT( !free_obj || obj != current_object );
    twc_DestroyObjectWindow( obj, free_obj);

    return 1;
}

/**
 * Set current object and load it's properties to property list.
 */
void SetCurrentObject( TWC_OBJECT *obj) /* object */
{
    TWC_OBJECT *prev_obj;

    TWC_CHECKIT( obj == NULL || IsObjectControl( obj) )
    TWC_CHECKIT( current_object == NULL || IsObjectControl( current_object) );

    /* Process changed properties */
    if ( current_object ) {
        ProcessChangedProperties();
    }

    prev_obj = current_object;
    current_object = obj;

    /* If current object is same as previous object, nothing to do */
    if ( current_object && current_object == prev_obj ) {
        return;
    }

    /* Redraw previous object if it is not window */
    if ( prev_obj && !IsObjectWindow( prev_obj) ) {
        RedrawWindow( prev_obj->hwnd, NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE | RDW_FRAME);
    }

    /* Redraw current object if it is not window */
    if ( current_object && current_object != prev_obj && !IsObjectWindow( current_object) ) {
        RedrawWindow( current_object->hwnd, NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE | RDW_FRAME);
    }

    /* Load current object properties to property list */
    if ( current_object ) {
        LoadPropertyList( hPropList, current_object);
    }
    return;
}
