#include "libtwc.h"

#include "properties.h"

#include "object.h"


static UINT client_data_size;

static LRESULT CALLBACK ObjectWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

/**
 * Set client data size for new objects.
 */
void twc_SetObjectsClientDataSize( UINT size) /* data size in bytes */
{
    client_data_size = size;
}

/**
 * Create new object.
 */
TWC_OBJECT *twc_NewObject( UINT ctrl_id) /* control ID */
{
    TWC_OBJECT *obj;

    /* Alloc memory for TWC_OBJECT */
    obj = (TWC_OBJECT *)calloc( 1, sizeof(TWC_OBJECT) + client_data_size - sizeof(CLIENT_DATA));

    /* Init */
    memset( obj, 0, sizeof(TWC_OBJECT) + client_data_size - sizeof(CLIENT_DATA));
    DListInit( &obj->child_list, sizeof(PTWC_OBJECT));

    obj->id = ctrl_id;
    if ( ctrl_id >= CONTROL_FIRST_ID ) {
        /* Alloc memory for properties */
        obj->classname = GetControlClassname( ctrl_id);
        obj->properties = (PROPERTY *)calloc( 1, GetControlPropertiesCount( ctrl_id) * sizeof(PROPERTY));
    }

    return obj;
}

/**
 * Copy object.
 */
TWC_OBJECT *twc_CopyObject( TWC_OBJECT *obj,    /* object to copy */
                            TWC_OBJECT *parent) /* parent of object's copy */
{
    TWC_OBJECT *new_obj;
    PROPERTY_INFO *propinfo;
    VALUE *val;
    UINT prop_id, prop_count;

    /* Create new object and copy obj bytewise */
    new_obj = twc_NewObject( obj->id);
    memcpy( new_obj, obj, sizeof(TWC_OBJECT) + client_data_size - sizeof(CLIENT_DATA));

    /* Copy classname */
    SetString( &(new_obj->classname), obj->classname, 1);
    /* Set parent */
    new_obj->parent = parent;

    /* Null some platform-specific fields and selection flag */
    new_obj->hwnd = 0;
    new_obj->orig_wndproc = NULL;
    
    /* Copy properties */
    prop_count = GetControlPropertiesCount( obj->id);
    new_obj->properties = malloc( prop_count * sizeof(PROPERTY));
    memcpy( new_obj->properties, obj->properties, prop_count * sizeof(PROPERTY));

    /* Copy strings for string properties */
    for ( prop_id = COMMON_PROPERTIES_BEGIN; prop_id < prop_count; prop_id++ ) {
        propinfo = GetPropertyInfo( obj->id, prop_id);
        val = GetObjectPropertyVal( new_obj, prop_id);
        if ( propinfo->type == T_STR ) {
            SetString( &(val->s), val->s, 1);
        }
    }

    /* Make title point to title property value */
    new_obj->title = GetObjectPropertyVal( new_obj, COMMON_TITLE)->s;

    /* Add object to parent's child list */
    new_obj->lstnode_ptr = DListAdd( twc_GetParentChildList( new_obj), (void *)-1, &new_obj);

    return new_obj;
}

/**
 * Free object.
 */
int twc_FreeObject( TWC_OBJECT *obj) /* object */
{
    PROPERTY_INFO *propinfo;
    UINT prop_id, prop_count;

    if ( obj == NULL ) {
        return 1;
    }

    TWC_CHECKIT( obj->hwnd == NULL );

    /* Free child objects */
    OBJ_LIST_ITERATE_BEGIN( &obj->child_list);
        twc_FreeObject( node->elem);
    OBJ_LIST_ITERATE_END();

    if ( IsObjectRoot( obj) ) {
        free( obj);
        return 1;
    }

    TWC_CHECKIT( obj->child_list.count == 0 );

    /* Free string values of properties */
    prop_count = GetControlPropertiesCount( obj->id);
    for ( prop_id = COMMON_PROPERTIES_BEGIN; prop_id < prop_count; prop_id++ ) {
        propinfo = GetPropertyInfo( obj->id, prop_id);
        if ( propinfo->type == T_STR ) {
            free( GetObjectPropertyVal( obj, prop_id)->s);
        }
    }
    free( obj->properties);

    /* Delete object from parent's child list */
    DListRemove( twc_GetParentChildList( obj), obj->lstnode_ptr);

    free( obj);
    return 1;
}

/**
 * Prepare object before creating it's window.
 *
 * Load default values for all properties and
 * set styles not covered by properties.
 */
void twc_PrepareObject( TWC_OBJECT *obj) /* object */
{
    /* Load defaults for all properties */
    SetObjectPropertyDefaultValue( obj, PROPERTIES_ALL);

    /* Set styles, witch not covered by properties */
    if ( obj->id == CTRL_ID_WINDOW ) {
        obj->style |= WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS/* | WS_CLIPCHILDREN*/;
        /* obj->exstyle = WS_EX_COMPOSITED */;
    } else {
        obj->style |= WS_CHILD/* | WS_CLIPSIBLINGS*/;
        switch ( obj->id ) {
            case CTRL_ID_GROUPBOX:
                obj->style |= BS_GROUPBOX;
                break;
        }
    }

    return;
}

/**
 * Object initialization after it's been created.
 */
static int InitObjectAfterCreation( TWC_OBJECT *obj,   /* object */
                                    HWND insert_after) /* insert after in z-order */
{
    /* Set OBJECT_INFO property and window procedure */
    SetProp( obj->hwnd, TEXT("OBJECT_INFO"), (HANDLE)obj);
    obj->orig_wndproc = (WNDPROC)SetWindowLongPtr( obj->hwnd, GWLP_WNDPROC, (LONG_PTR)ObjectWndProc);
    SetWindowPos( obj->hwnd, insert_after, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
    SendMessage( obj->hwnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), 0);

    if ( obj->id != CTRL_ID_WINDOW ) {
        /* Set control position (need for some controls to do it twice) and set font */
        SetWindowPos( obj->hwnd, NULL, obj->x, obj->y, obj->width, obj->height, SWP_NOZORDER);
    }

    return 1;
}

/**
 * Create object window.
 */
int twc_CreateObjectWindow( TWC_OBJECT *obj,        /* object */
                            TWC_BOOL create_childs, /* TWC_TRUE for create childs */
                            HWND insert_after)
{
    RECT rect;

    /* Make object window rect */
    rect.left = rect.top = 0;
    rect.right = obj->width;
    rect.bottom = obj->height;
    if ( obj->flags & OBJ_FLAG_CLIENTSIZE ) {
        AdjustWindowRectEx( &rect, obj->style, FALSE, obj->exstyle);
    }

    /* Create window */
    obj->hwnd = CreateWindowEx( obj->exstyle, obj->classname, obj->title, obj->style,
                                obj->x, obj->y, rect.right - rect.left, rect.bottom - rect.top, obj->parent->hwnd, NULL, GetModuleHandle(NULL), obj);
    if ( obj->hwnd == NULL ) {
        return 0;
    }

    /* Initialize object */
    if ( InitObjectAfterCreation( obj, insert_after) == 0 ) {
        return 0;
    }

    /* Create childs */
    if ( create_childs ) {
        OBJ_LIST_ITERATE_BEGIN( &obj->child_list);
            if ( twc_CreateObjectWindow( node->elem, TWC_TRUE, obj->hwnd) == 0 ) {
                return 0;
            }
        OBJ_LIST_ITERATE_END();
    }

    return 1;
}

/**
 * Destroy object window and free object (if <free_obj> TRUE).
 */
int twc_DestroyObjectWindow( TWC_OBJECT *obj,   /* object */
                             TWC_BOOL free_obj) /* TWC_TRUE for free object */
{
    TWC_CHECKIT( obj != NULL );

    /* Destroy childs */
    if ( obj->child_list.count != 0 ) {
        OBJ_LIST_ITERATE_BEGIN( &obj->child_list);
            twc_DestroyObjectWindow( node->elem, free_obj);
        OBJ_LIST_ITERATE_END();
    }

    DestroyWindow( obj->hwnd);

    if ( free_obj ) {
        twc_FreeObject( obj);
    }

    return 1;
}

/**
 * Destroy and create object window
 */
int twc_ReCreateObjectWindow( TWC_OBJECT *obj) /* object */
{
	HWND insert_after;

	if ( obj->hwnd == 0 ) return 0;

	insert_after = GetNextWindow( obj->hwnd, GW_HWNDPREV);

	twc_DestroyObjectWindow( obj, TWC_FALSE);
    twc_CreateObjectWindow( obj, TWC_FALSE, insert_after);
    //TODO: insert in zorder
	return 1;
}

/**
 * Get parent object child list.
 */
DLIST_PTWC_OBJECT *twc_GetParentChildList( TWC_OBJECT *obj) /* object */
{
    TWC_CHECKIT( !IsObjectRoot( obj) );
    return &obj->parent->child_list;
}

/**
 * Object window procedure.
 */
static LRESULT CALLBACK ObjectWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    TWC_OBJECT *obj;
    RECT rect;

    obj = GetProp( hwnd, T("OBJECT_INFO"));

    switch ( msg ) {
        case WM_NCDESTROY:
            RemoveProp( hwnd, T("OBJECT_INFO"));
            obj->hwnd = NULL;
            break;
        case WM_MOVE:
            /* Write new values to properties */
			GetWindowRect( hwnd, &rect);
			MapWindowPoints( HWND_DESKTOP, GetParent(hwnd), (POINT *)&rect, 1);
			SetObjectPropertyInt(obj, COMMON_X, rect.left, TWC_TRUE, TWC_FALSE);
            SetObjectPropertyInt(obj, COMMON_Y, rect.top, TWC_TRUE, TWC_FALSE);
			break;
		case WM_SIZE:
            /* Write new values to properties */
            if ( obj->flags & OBJ_FLAG_CLIENTSIZE ) {
			    GetClientRect( hwnd, &rect);
            } else {
                GetWindowRect( hwnd, &rect);
            }
			SetObjectPropertyInt( obj, COMMON_WIDTH, rect.right - rect.left, TWC_TRUE, TWC_FALSE);
			SetObjectPropertyInt( obj, COMMON_HEIGHT, rect.bottom - rect.top, TWC_TRUE, TWC_FALSE);
			break;
    }

    if ( obj->client_wndproc != NULL )
    {
        return obj->client_wndproc( obj, msg, wParam, lParam);
    } else
    {
        return CallWindowProc( obj->orig_wndproc, hwnd, msg, wParam, lParam);
    }
}