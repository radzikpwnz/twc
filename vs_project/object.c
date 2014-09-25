#include <windows.h>
#include <tchar.h>

#include "common.h"

#include "..\property_list\proplist.h"
#include "proplst.h"
#include "properties.h"
#include "interface.h"
#include "window.h"
#include "static.h"

#include "object.h"

/**
 * Create new object.
 */
RT_OBJECT *NewObject( UINT ctrl_id) /* control ID */
{
    RT_OBJECT *obj;

    /* Alloc memory for RT_OBJECT */
    obj = (RT_OBJECT *)calloc( 1, sizeof(RT_OBJECT));

    /* Init */
    memset( obj, 0, sizeof(RT_OBJECT));
    DListInit( &obj->child_list, sizeof(PRT_OBJECT));

    obj->ctrl_id = ctrl_id;
    if ( ctrl_id != CTRL_ID_UNDEFINED ) {
        /* Alloc memory for properties */
        obj->classname = GetControlClassname( ctrl_id);
        obj->properties = (PROPERTY *)calloc( 1, GetControlPropertiesCount( ctrl_id) * sizeof(PROPERTY));
    }

    return obj;
}

/**
 * Copy object.
 */
RT_OBJECT *CopyObject( RT_OBJECT *obj,    /* object to copy */
                       RT_OBJECT *parent) /* parent of object's copy */
{
    RT_OBJECT *new_obj;
    PROPERTY_INFO *propinfo;
    VALUE *val;
    UINT prop_id, prop_count;

    /* Create new object and copy obj bytewise */
    new_obj = NewObject( obj->ctrl_id);
    memcpy( new_obj, obj, sizeof(RT_OBJECT));

    /* Copy classname */
    new_obj->classname = obj->classname; //TODO: COPY!
    /* Set parent */
    new_obj->parent = parent;

    /* Null some window-specific fields and selection flag*/
    new_obj->hwnd = 0;
    new_obj->selected = 0;
    new_obj->orig_wndproc = NULL;
    new_obj->static_orig_wndproc = NULL;
    new_obj->static_hwnd = NULL;

    /* Copy properties */
    prop_count = GetControlPropertiesCount( obj->ctrl_id);
    new_obj->properties = malloc( prop_count * sizeof(PROPERTY));
    memcpy( new_obj->properties, obj->properties, prop_count * sizeof(PROPERTY));

    /* Copy strings for string properties */
    for ( prop_id = COMMON_PROPERTIES_BEGIN; prop_id < prop_count; prop_id++ ) {
        propinfo = GetPropertyInfo( obj->ctrl_id, prop_id);
        val = GetObjectPropertyVal( new_obj, prop_id);
        if ( propinfo->type == T_STR ) {
            SetString( &(val->s), val->s, 1);
        }
    }

    /* Generate object name and make title point to title property value */
    GenerateObjectName(new_obj);
    new_obj->title = GetObjectPropertyVal(new_obj, COMMON_TITLE)->s;

    /* Add object to parent's child list */
    new_obj->lstnode_ptr = DListAdd( GetParentChildList( new_obj), (void *)-1, &new_obj);

    return new_obj;
}

/**
 * Free object.
 */
int FreeObject( RT_OBJECT *obj) /* object */
{
    PROPERTY_INFO *propinfo;
    UINT prop_id, prop_count;

    if (obj == NULL) return 1;

    TWC_CHECKIT( obj != current_object );

    /* Free child objects */
    OBJ_LIST_ITERATE_BEGIN( &obj->child_list);
        FreeObject( node->elem);
    OBJ_LIST_ITERATE_END();

    TWC_CHECKIT( obj->child_list.count == 0 );

    /* Free string values of properties */
    prop_count = GetControlPropertiesCount( obj->ctrl_id);
    for ( prop_id = COMMON_PROPERTIES_BEGIN; prop_id < prop_count; prop_id++ ) {
        propinfo = GetPropertyInfo( obj->ctrl_id, prop_id);
        if ( propinfo->type == T_STR ) {
            free( GetObjectPropertyVal( obj, prop_id)->s);
        }
    }
    free( obj->properties);

    /* Delete object from parent's child list */
    DListRemove( GetParentChildList( obj), obj->lstnode_ptr);

    free(obj);

    return 1;
}

/**
 * Prepare object before creating it's window.
 *
 * Load default values for all properties and
 * set styles not covered by properties.
 */
void PrepareObject( RT_OBJECT *obj) /* object */
{
    /* Load defaults for all properties */
    SetObjectPropertyDefaultValue( obj, PROPERTIES_ALL);

    /* Set styles, witch not covered by properties */
    if ( obj->ctrl_id == CTRL_ID_WINDOW ) {
        obj->style |= WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS/* | WS_CLIPCHILDREN*/;
        /* obj->exstyle = WS_EX_COMPOSITED */;
    } else {
        obj->style |= WS_CHILD/* | WS_CLIPSIBLINGS*/;
        switch ( obj->ctrl_id ) {
            case CTRL_ID_GROUPBOX:
                obj->style |= BS_GROUPBOX;
                break;
        }
    }
    return;
}

/**
 * Set default values for new object.
 */
void SetNewObjectDefaultValues( RT_OBJECT *obj) /* object */
{
    switch ( obj->ctrl_id ) {
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
static int CreateStaticOnControl( RT_OBJECT *obj) /* object */
{
    HWND hwnd;

    TWC_CHECKIT( obj->ctrl_id != CTRL_ID_WINDOW );

    hwnd = CreateWindowEx( 0, WC_STATIC, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, obj->x, obj->y, obj->width, obj->height,
                    obj->parent->hwnd, NULL, GetModuleHandle(NULL), 0);
    if ( hwnd == NULL ) {
        return 0;
    }
    obj->static_hwnd = hwnd;

    /* Set OBJECT_INFO property and window procedure */
    SetProp( hwnd, T("OBJECT_INFO"), (HANDLE)obj);
    obj->static_orig_wndproc = (WNDPROC)SetWindowLongPtr( hwnd, GWLP_WNDPROC, (LONG_PTR)StaticWndProc);

    /* Set z-order */
    SetWindowPos( hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE); //TODO: find exact zorder

    return 1;
}

/**
 * Object initialization after it's been created.
 */
static int InitObjectAfterCreation( RT_OBJECT *obj,    /* object */
                                    HWND insert_after) /* insert after in z-order */
{
    /* Set OBJECT_INFO property and window procedure */
    SetProp( obj->hwnd, TEXT("OBJECT_INFO"), (HANDLE)obj);
    obj->orig_wndproc = (WNDPROC)SetWindowLongPtr( obj->hwnd, GWLP_WNDPROC,
                        (( obj->ctrl_id != CTRL_ID_WINDOW ) ? (LONG_PTR)ControlWndProc : (LONG_PTR)ChildWndProc) );

    if ( obj->ctrl_id != CTRL_ID_WINDOW ) {
        /* Set control position (need for some controls to do it twice), create static and set font */
        SetWindowPos( obj->hwnd, insert_after, obj->x, obj->y, obj->width, obj->height, 0);
        CreateStaticOnControl( obj);
        SendMessage( obj->hwnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), 0);
    }

    return 1;
}

/**
 * Create object window.
 */
int CreateObjectWindow( RT_OBJECT *obj,         /* object */
                        TWC_BOOL create_childs) /* TWC_TRUE for create childs */
{
    HWND parent_hwnd;
    DWORD mdi_style;
    RECT rect;

    /* Determine parent HWND and MDI style */
    parent_hwnd = (obj->parent == NULL) ? MDIClient.hwnd : obj->parent->hwnd;
    mdi_style = ( parent_hwnd == MDIClient.hwnd ) ? WS_EX_MDICHILD : 0;

    /* Make object window rect */
    rect.left = rect.top = 0;
    rect.right = obj->width;
    rect.bottom = obj->height;
    if ( obj->flags & OBJ_FLAG_CLIENTSIZE ) {
        AdjustWindowRectEx( &rect, obj->style, FALSE, obj->exstyle);
    }

    /* Create window */
    obj->hwnd = CreateWindowEx( obj->exstyle | mdi_style, obj->classname, obj->title, StyleFilter(obj),
                                obj->x, obj->y, rect.right - rect.left, rect.bottom - rect.top, parent_hwnd, NULL, GetModuleHandle(NULL), obj);
    if ( obj->hwnd == NULL ) {
        return 0;
    }

    /* Initialize object */
    if ( InitObjectAfterCreation( obj, HWND_TOP) == 0 ) {
        return 0;
    }

    /* Create childs */
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
int DestroyObjectWindow( RT_OBJECT *obj,    /* object */
                         TWC_BOOL free_obj) /* TWC_TRUE for free object */
{
    TWC_CHECKIT( obj != NULL );
    TWC_CHECKIT( !free_obj || obj != current_object );

    /* Destroying childs */
    if ( obj->child_list.count != 0 ) {
        OBJ_LIST_ITERATE_BEGIN( &obj->child_list);
            DestroyObjectWindow( node->elem, free_obj);
        OBJ_LIST_ITERATE_END();
    }

    DestroyWindow( obj->hwnd);

    if ( free_obj ) {
        FreeObject( obj);
    }

    return 1;
}

/**
 * Set current object and load it's properties to property list.
 */
void SetCurrentObject( RT_OBJECT *obj) /* object */
{
    RT_OBJECT *prev_obj;

    /* Process changed properties */
    if ( current_object ) {
        SendMessage( hPropList, PL_PROCESSCHANGEDPROP, 0, 0);
    }

    prev_obj = current_object;
    current_object = obj;

    /* If current object is same as previous object, nothing to do */
    if ( current_object && current_object == prev_obj ) {
        return;
    }

    /* Redraw previous object if it is not window */
    if ( prev_obj && prev_obj->ctrl_id != CTRL_ID_WINDOW ) {
        RedrawWindow( prev_obj->hwnd, NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE | RDW_FRAME);
    }

    /* Redraw current object if it is not window */
    if ( current_object && current_object != prev_obj && current_object->ctrl_id != CTRL_ID_WINDOW ) {
        RedrawWindow( current_object->hwnd, NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE | RDW_FRAME);
    }

    /* Load current object properties to property list */
    if ( current_object ) {
        LoadPropertyList( hPropList, current_object);
    }
    return;
}

/**
 * Get parent object child list.
 */
DLIST_PRT_OBJECT *GetParentChildList( RT_OBJECT *obj) /* object */
{
    return ( obj->parent == NULL ) ? &cur_project.obj_list : &obj->parent->child_list;
}
