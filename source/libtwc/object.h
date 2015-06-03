#ifndef LIBTWC_OBJECT_H
#define LIBTWC_OBJECT_H

#include "libtwc.h"

typedef struct tagTWC_OBJECT TWC_OBJECT, *PTWC_OBJECT;
DLIST_CUSTOM( PTWC_OBJECT);

#include "properties.h"

/**
 * Object flags
 */
enum {
    OBJ_FLAG_CLIENTSIZE = 1 << 0 /* use client size instead window size */
};

typedef LRESULT (CALLBACK* TWC_CLIENTPROC)(TWC_OBJECT*, UINT, WPARAM, LPARAM);

typedef UINT CLIENT_DATA;

/**
 * Main object info structure
 */
struct tagTWC_OBJECT {
    UINT id;                            /* object ID */
    DLIST_PTWC_OBJECT child_list;       /* child list */

    int x, y;                           /* position */
    int width, height;                  /* size */

    TWC_OBJECT *parent;                 /* parent object (NULL for window) */
    PROPERTY *properties;               /* properties array */
    
    TCHAR *classname;                   /* class name */
    HWND hwnd;                          /* object window HWND */          
    WNDPROC orig_wndproc;               /* original window procedure */
    TWC_CLIENTPROC client_wndproc;      /* client window procedure */

    DWORD style;                        /* style and extended style */
    DWORD exstyle;

    TCHAR *name;                        /* name */
    TCHAR *title;                       /* title or text */
    UINT flags;                         /* flags */

    DLIST_NODE_PTWC_OBJECT *lstnode_ptr; /* pointer to list node, containing object */

    CLIENT_DATA client_data[1];
};

/* Set client data size for new objects */
EXPORT void twc_SetObjectsClientDataSize( UINT size);

/* Create new object */
EXPORT TWC_OBJECT *twc_NewObject( UINT ctrl_id);

/* Copy object */
EXPORT TWC_OBJECT *twc_CopyObject( TWC_OBJECT *obj, TWC_OBJECT *parent);

/* Free object */
EXPORT int twc_FreeObject( TWC_OBJECT *obj);

/* Prepare object before creating it's window */
EXPORT void twc_PrepareObject( TWC_OBJECT *obj);

/* Create object window */
EXPORT int twc_CreateObjectWindow( TWC_OBJECT *obj, TWC_BOOL create_childs, HWND insert_after);

/* Destroy object window and free object (if free_obj == TRUE) */
EXPORT int twc_DestroyObjectWindow( TWC_OBJECT *obj, TWC_BOOL free_obj);

/* Destroy and create object window */
EXPORT int twc_ReCreateObjectWindow( TWC_OBJECT *obj);

/* Get parent object child list */
EXPORT DLIST_PTWC_OBJECT *twc_GetParentChildList( TWC_OBJECT *obj);


/**
 * Iterators for TWC_OBJECT lists.
 */
#define NODE() (__node)

#define OBJ_LIST_ITERATE_BEGIN( list )       \
{                                            \
    DLIST_NODE_PTWC_OBJECT *__next, *NODE(); \
    NODE() = (list)->first;                  \
    while ( NODE() != NULL ) {               \
        __next = NODE()->next;

#define OBJ_LIST_ITERATE_BEGIN_FROM_NODE( start_node )  \
{                                            \
    DLIST_NODE_PTWC_OBJECT *__next, *NODE(); \
    NODE() = (start_node);                   \
    while ( NODE() != NULL ) {               \
        __next = NODE()->next;

#define OBJ_LIST_ITERATE_END()               \
        NODE() = __next;                     \
    }                                        \
}


/* If object is control (incl window) or not */
#define IsObjectControl( obj) ( (obj)->id >= CONTROL_FIRST_ID )

#define IsObjectWindow( obj) ( (obj)->id == CTRL_ID_WINDOW )

/* If object is root */
#define IsObjectRoot( obj) ( (obj)->id == CTRL_ID_ROOT )

/* If object is undefined */
#define IsObjectUndefined( obj) ( (obj)->id == CTRL_ID_UNDEFINED )


#endif