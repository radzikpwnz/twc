#ifndef OBJECT_H
#define OBJECT_H

#include <windows.h>

#include "mylist.h"

typedef struct tagRT_OBJECT RT_OBJECT, *PRT_OBJECT;
DLIST_CUSTOM( PRT_OBJECT);
#define RT_CONTROL RT_OBJECT
#define RT_WINDOW RT_OBJECT

#include "properties.h"
#include "code_window.h"

/**
 * Object flags
 */
enum {
    OBJ_FLAG_CLIENTSIZE = 1 << 0 /* use client size instead window size */
};

/**
 * Main object info structure
 */
struct tagRT_OBJECT {
    UINT id;                        /* object ID */
    DLIST_PRT_OBJECT child_list;    /* child list */

    int selected;                   /* if object selected */ 
    int x, y;                       /* position */
    int width, height;              /* size */

    RT_OBJECT *parent;              /* parent object (NULL for window) */
    PROPERTY *properties;           /* properties array */
    
    TCHAR *classname;               /* class name */
    HWND hwnd;                      /* object and static HWND */ 
    HWND static_hwnd;           
    WNDPROC orig_wndproc;           /* object and static original window procedures */
    WNDPROC static_orig_wndproc;
    DWORD style;                    /* style and extended style */
    DWORD exstyle;

    TCHAR *name;                    /* name */
    TCHAR *title;                   /* title or text */
    UINT flags;                     /* flags */

    /* pointer to list node, containing object */
    DLIST_NODE_PRT_OBJECT *lstnode_ptr;
      
    CODE_WINDOW *code_window;       /* code window if exists */
};

/**
 * Create new object.
 */
RT_OBJECT *NewObject( UINT ctrl_id);

/**
 * Copy object.
 */
RT_OBJECT *CopyObject( RT_OBJECT *obj, RT_OBJECT *parent);

/**
 * Free object.
 */
int FreeObject( RT_OBJECT *obj);

/**
 * Prepare object before creating it's window.
 */
void PrepareObject( RT_OBJECT *obj);

/**
 * Set default values for new object.
 */
void SetNewObjectDefaultValues( RT_OBJECT *obj);

/**
 * Create object window.
 */
int CreateObjectWindow( RT_OBJECT *obj, TWC_BOOL create_childs);

/**
 * Destroy object window and free object (if <free_obj> TRUE).
 */
int DestroyObjectWindow( RT_OBJECT *obj, TWC_BOOL free_obj);

/**
 * Set current object and load it's properties to property list.
 */
void SetCurrentObject( RT_OBJECT *obj);


/**
 * Iterators for RT_OBJECT lists.
 */
#define OBJ_LIST_ITERATE_BEGIN( list )  \
{                                       \
    DLIST_NODE_PRT_OBJECT *next, *node; \
    node = (list)->first;               \
    while ( node != NULL ) {            \
        next = node->next;

#define OBJ_LIST_ITERATE_BEGIN_FROM_NODE( start_node )  \
{                                       \
    DLIST_NODE_PRT_OBJECT *next, *node; \
    node = start_node;                  \
    while ( node != NULL ) {            \
        next = node->next;

#define OBJ_LIST_ITERATE_END()          \
        node = next;                    \
    }                                   \
}

#endif


/* If object is control (incl window) or not */
#define IsObjectControl( obj) ( (obj)->obj_id >= CONTROL_FIRST_ID )

/* If object is root */
#define IsObjectRoot( obj) ( (obj)->obj_id == CTRL_ID_ROOT )

/* If object is undefined */
#define IsObjectUndefined( obj) ( (obj)->obj_id == CTRL_ID_UNDEFINED )