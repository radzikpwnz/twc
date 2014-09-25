#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <windows.h>

typedef struct tagPROPERTY PROPERTY;
DLIST_CUSTOM_FD(PROPERTY);

#include "common.h"

#include "ctrl_info.h"

enum {
    PROPERTY_FLAG_SET = 1 << 0,
    PROPERTY_FLAG_DEFAULT = 1 << 1
};

struct tagPROPERTY {
	UINT flags;
	VALUE val;
};

DLIST_CUSTOM_RD(PROPERTY);

int ApplyObjectProperty( RT_OBJECT *obj, UINT on_apply_act);

int UpdateObjectProperty( RT_OBJECT *obj, UINT prop_id, BOOL apply);
/*Apply change of property prop_id to object. Set prop_id = -1 to load defaults and update all properties*/

PROPERTY *GetObjectProperty(RT_OBJECT *obj, UINT prop_id);
VALUE *GetObjectPropertyVal(RT_OBJECT *obj, int prop_id);
UINT GetObjectPropertyFlags(RT_OBJECT *obj, int prop_id);

int SetObjectProperty(RT_OBJECT *obj, UINT prop_id, const VALUE *new_val, BOOL update, BOOL apply);
int SetObjectPropertyInt( RT_OBJECT *obj, UINT prop_id, int val, BOOL update, BOOL apply);
int SetObjectPropertyStr( RT_OBJECT *obj, UINT prop_id, TCHAR *val, BOOL update, BOOL apply);
/*Sets object's property, update != 0 to update this property*/

int SetObjectPropertyDefaultValue( RT_OBJECT *obj, UINT prop_id);


enum tagPROPFILTER_CODE {
	PF_WRONG,
	PF_CORRECTED,
	PF_DEFAULT,
	PF_OK
} PROPFILTER_CODE;

int PropertyFilter(RT_OBJECT *obj, int prop_id, VALUE *val);
int GenerateObjectName(RT_OBJECT *obj);
int StyleFilter(RT_OBJECT *obj);


#endif