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


EXPORT PROPERTY *GetObjectProperty( TWC_OBJECT *obj, UINT prop_id);
EXPORT VALUE *GetObjectPropertyVal( TWC_OBJECT *obj, int prop_id);
EXPORT UINT GetObjectPropertyFlags( TWC_OBJECT *obj, int prop_id);

EXPORT int SetObjectProperty( TWC_OBJECT *obj, UINT prop_id, const VALUE *new_val, TWC_BOOL update, TWC_BOOL apply);
EXPORT int SetObjectPropertyInt( TWC_OBJECT *obj, UINT prop_id, int val, TWC_BOOL update, TWC_BOOL apply);
EXPORT int SetObjectPropertyStr( TWC_OBJECT *obj, UINT prop_id, const TCHAR *val, TWC_BOOL update, TWC_BOOL apply);

EXPORT int SetObjectPropertyDefaultValue( TWC_OBJECT *obj, UINT prop_id);


#endif
