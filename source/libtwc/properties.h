#ifndef LIBTWC_PROPERTIES_H
#define LIBTWC_PROPERTIES_H

typedef struct tagPROPERTY PROPERTY;
DLIST_CUSTOM_FD( PROPERTY);

#include "libtwc.h"

#include "object.h"
#include "ctrl_info.h"

/* Property flags */
enum {
    PROPERTY_FLAG_SET = 1 << 0,    /* set */
    PROPERTY_FLAG_DEFAULT = 1 << 1 /* default */
};

/* Property */
struct tagPROPERTY {
    VALUE val;  /* value */
	UINT flags; /* flags */
};


DLIST_CUSTOM_RD( PROPERTY);


EXPORT PROPERTY *twc_GetObjectProperty( TWC_OBJECT *obj, UINT prop_id);
EXPORT VALUE *twc_GetObjectPropertyVal( TWC_OBJECT *obj, int prop_id);
EXPORT UINT twc_GetObjectPropertyFlags( TWC_OBJECT *obj, int prop_id);

EXPORT int twc_SetObjectProperty( TWC_OBJECT *obj, UINT prop_id, const VALUE *new_val, TWC_BOOL update, TWC_BOOL apply);
EXPORT int twc_SetObjectPropertyInt( TWC_OBJECT *obj, UINT prop_id, int val, TWC_BOOL update, TWC_BOOL apply);
EXPORT int twc_SetObjectPropertyStr( TWC_OBJECT *obj, UINT prop_id, const TCHAR *val, TWC_BOOL update, TWC_BOOL apply);

EXPORT int twc_SetObjectPropertyDefaultValue( TWC_OBJECT *obj, UINT prop_id);


#endif
