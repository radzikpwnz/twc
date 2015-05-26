#include <windows.h>

#include "twc_design.h"

#include "properties.h"

#include "proplst.h"

static PL_VALTYPE valtype_to_pl_valtype[] = {
    PL_INT,  /* T_INT */
    PL_TEXT, /* T_STR */
    PL_LIST, /* T_LIST */
    PL_BOOL  /* T_BOOL */
};

static void InitItemInfoByValue( PL_ITEMINFO* iteminfo, const VALUE *val, UINT val_type)
{
    iteminfo->type = valtype_to_pl_valtype[val_type];

    switch ( val_type ) {
        case T_INT:
        case T_LIST:
        case T_BOOL:
			iteminfo->val.i = val->i;
			break;		
		case T_STR:
			iteminfo->val.s = val->s;
			break;
    }
    return;
}

int UpdateSingleListProperty(HWND hPropList, TWC_OBJECT *obj, UINT prop_id, const VALUE *val)
{
	PL_ITEMINFO iteminfo;

	iteminfo.id = prop_id;

    InitItemInfoByValue( &iteminfo, val, GetPropertyInfo( obj->id, prop_id)->type);

	SendMessage(hPropList, PL_SETITEMVALUE, (WPARAM)&iteminfo, 0);
	return 1;
}

static int AddSingleProperty(HWND hPropList, TWC_OBJECT *obj, UINT prop_id)
{
	PL_ITEMINFO iteminfo;
    PROPERTY_INFO *propinfo;
    VALUE *val;
    
    propinfo = GetPropertyInfo( obj->id, prop_id);
    val = GetObjectPropertyVal( obj, prop_id);

	iteminfo.name = propinfo->dname;
	iteminfo.id = prop_id;
    iteminfo.type = valtype_to_pl_valtype[propinfo->type];

	switch (propinfo->type) {
		case T_INT:
		case T_BOOL:
			iteminfo.val.i = val->i;
			break;
		case T_STR:
			iteminfo.val.s = val->s;
			break;
		case T_LIST:
			iteminfo.val.h = (HANDLE)propinfo->opt;
			SendMessage( hPropList, PL_ADDITEM, (WPARAM)&iteminfo, MAKELPARAM(val->i, 0));
			return 1;
	}
	
	SendMessage( hPropList, PL_ADDITEM, (WPARAM)&iteminfo, 0);
	return 1;
}

int LoadPropertyList(HWND hPropList, TWC_OBJECT *obj)
{
	UINT prop_count;
    UINT prop_id;

	SendMessage( hPropList, PL_STARTFILL, 0, 0);

    prop_count = GetControlPropertiesCount( obj->id);

    for ( prop_id = COMMON_PROPERTIES_BEGIN; prop_id < prop_count; prop_id++ ) {
        AddSingleProperty( hPropList, obj, prop_id);
    }

	SendMessage(hPropList, PL_ENDFILL, 0, 0);
	return 1;
}

int OnChangeListProperty(HWND hPropList, TWC_OBJECT *obj, UINT prop_id)
{
	VALUE val;
	int code;

	if (obj == NULL) return 0;
	SendMessage(hPropList, PL_GETITEMVALUE, prop_id, (LPARAM)&val);
	code = PropertyFilter(obj, prop_id, &val);
	switch (code) {
		//case PF_DEFAULT:
			//RemoveProperty(obj, prop_id);
			//break;
		case PF_CORRECTED:
			UpdateSingleListProperty(hPropList, obj, prop_id, &val);
		case PF_OK:
        case PF_DEFAULT:
			SetObjectProperty(obj, prop_id, &val, TWC_TRUE, TWC_TRUE);
			break;
		case PF_WRONG:
			return 0;
	}
	return 1;
}

int ProcessChangedProperties()
{
    SendMessage( hPropList, PL_PROCESSCHANGEDPROP, 0, 0);
    return 1;
}