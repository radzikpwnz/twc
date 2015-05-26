#ifndef PROPERTIES_H
#define PROPERTIES_H

typedef struct tagPROPERTY PROPERTY;
DLIST_CUSTOM_FD( PROPERTY);

#include "twc_design.h"
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

/* Codes returned by PropertyFilter */
enum tagPROPFILTER_CODE {
	PF_WRONG,     /* wrong value */
	PF_CORRECTED, /* wrong value, but it has been corrected */
	PF_DEFAULT,   /* value default value for this property */
	PF_OK         /* value ok */
} PROPFILTER_CODE;


DLIST_CUSTOM_RD( PROPERTY);

int GenerateObjectName( RT_OBJECT *obj);
int StyleFilter( RT_OBJECT *obj);

int ApplyObjectProperty( RT_OBJECT *obj, UINT on_apply_act);

int UpdateObjectProperty( RT_OBJECT *obj, UINT prop_id, TWC_BOOL apply);

int PropertyFilter(RT_OBJECT *obj, int prop_id, VALUE *val);

PROPERTY *GetObjectProperty( RT_OBJECT *obj, UINT prop_id);
VALUE *GetObjectPropertyVal( RT_OBJECT *obj, int prop_id);
UINT GetObjectPropertyFlags( RT_OBJECT *obj, int prop_id);

int SetObjectProperty( RT_OBJECT *obj, UINT prop_id, const VALUE *new_val, TWC_BOOL update, TWC_BOOL apply);
int SetObjectPropertyInt( RT_OBJECT *obj, UINT prop_id, int val, TWC_BOOL update, TWC_BOOL apply);
int SetObjectPropertyStr( RT_OBJECT *obj, UINT prop_id, const TCHAR *val, TWC_BOOL update, TWC_BOOL apply);

int SetObjectPropertyDefaultValue( RT_OBJECT *obj, UINT prop_id);


#endif
