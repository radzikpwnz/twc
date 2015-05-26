#ifndef OBJECT_H
#define OBJECT_H

#include "twc_design.h"

#include "code_window.h"

/**
 * Main object info structure
 */
typedef struct tagTWC_OBJECT_CLIENT_DATA {
  
    TWC_BOOL selected;                   /* if object selected */   
    HWND static_hwnd;           
    WNDPROC static_orig_wndproc;      
    CODE_WINDOW *code_window;       /* code window if exists */
} TWC_OBJECT_CLIENT_DATA;

#define OBJ_CLIENT_DATA( obj) ((TWC_OBJECT_CLIENT_DATA *)(&(obj->client_data)))

/**
 * Copy object.
 */
TWC_OBJECT *CopyObject( TWC_OBJECT *obj, TWC_OBJECT *parent);

/**
 * Free object.
 */
int FreeObject( TWC_OBJECT *obj);

/**
 * Set default values for new object.
 */
void SetNewObjectDefaultValues( TWC_OBJECT *obj);

/**
 * Create object window.
 */
int CreateObjectWindow( TWC_OBJECT *obj, TWC_BOOL create_childs);

/**
 * Destroy object window and free object (if <free_obj> TRUE).
 */
int DestroyObjectWindow( TWC_OBJECT *obj, TWC_BOOL free_obj);

/**
 * Set current object and load it's properties to property list.
 */
void SetCurrentObject( TWC_OBJECT *obj);

#endif