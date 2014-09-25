#ifndef OBJECT_H
#define OBJECT_H

typedef struct tagRT_OBJECT RT_OBJECT, *PRT_OBJECT;
DLIST_CUSTOM(PRT_OBJECT);
#define RT_CONTROL RT_OBJECT
#define RT_WINDOW RT_OBJECT

#include "code_window.h"
#include "properties.h"

enum {
    OBJ_FLAG_CLIENTSIZE = 1 << 0
};

struct tagRT_OBJECT {
    TCHAR *name;

    TCHAR *classname;
    int x, y;
    int width, height;
    TCHAR *title;
    DWORD style;
    DWORD exstyle;
    HWND hwnd;
    UINT flags;

    UINT ctrl_id;
    PROPERTY *properties;

    DLIST_NODE_PRT_OBJECT *lstnode_ptr;

    RT_OBJECT *parent;
    DLIST_PRT_OBJECT child_list;

    int selected;
    CODE_WINDOW *code_window;

    WNDPROC orig_wndproc;
    WNDPROC static_orig_wndproc;
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