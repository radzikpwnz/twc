#ifndef OBJECT_H
#define OBJECT_H

typedef struct tagRT_OBJECT RT_OBJECT, *PRT_OBJECT;
DLIST_CUSTOM(PRT_OBJECT);
#define RT_CONTROL RT_OBJECT
#define RT_WINDOW RT_OBJECT

#include "code_window.h"
#include "properties.h"

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
    /* name */
    TCHAR *name;

    /* class name */
    TCHAR *classname;
    /* position and size*/
    int x, y;
    int width, height;
    /* title or text */
    TCHAR *title;
    /* style and extended style */
    DWORD style;
    DWORD exstyle;
    /* window HWND */
    HWND hwnd;
    /* flags */
    UINT flags;

    /* control ID */
    UINT ctrl_id;
    /* properties array */
    PROPERTY *properties;

    /* pointer to list node, containing object */
    DLIST_NODE_PRT_OBJECT *lstnode_ptr;

    /* parent object (NULL for window) */
    RT_OBJECT *parent;
    /* child list */
    DLIST_PRT_OBJECT child_list;

    /* if object selected */
    int selected;
    CODE_WINDOW *code_window;

    /* object and static original window procedures */
    WNDPROC orig_wndproc;
    WNDPROC static_orig_wndproc;

    /* static HWND */
    HWND static_hwnd;
};


RT_OBJECT *NewObject( UINT ctrl_id);
RT_OBJECT *CopyObject(RT_OBJECT *obj, RT_OBJECT *parent);
int FreeObject(RT_OBJECT *obj);

void PrepareObject( RT_OBJECT *obj);
void SetNewObjectDefaultValues( RT_OBJECT *obj);

int CreateObjectWindow( RT_OBJECT *obj, TWC_BOOL create_childs);
int DestroyObjectWindow( RT_OBJECT *obj, TWC_BOOL free_obj);

void SetCurrentObject(RT_OBJECT *obj);
DLIST_PRT_OBJECT *GetParentChildList( RT_OBJECT *obj);


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
