#include "twc_design.h"

#include "properties.h"


/* Objects name usage count for each object id (used to generate unique name) */
UINT control_names_usage[CONTROL_COUNT];

/**
 * Generate unique object name.
 */
int GenerateObjectName( TWC_OBJECT *obj) /* object */
{
	VALUE val;
	TCHAR buf[64];
	DLIST_NODE_PTWC_OBJECT *obj_node;
	int ok;
    TCHAR *default_name;

    /* Get default object name */
    default_name = GetControlDefaultObjectName( obj->id);
	ok = 0;

    /* Add number after default name until it become unique */
next:
	while (!ok) {
		_stprintf( buf, T("%s%d"), default_name, ++control_names_usage[obj->id]);

        obj_node = twc_GetParentChildList( obj)->first;
		while (obj_node != NULL) {
			if ( obj_node->elem->name
                 && _tcscmp( obj_node->elem->name, buf) == 0
                 && obj_node->elem != obj ) {
				ok = 0;
				goto next;
			}
			ok = 1;
			obj_node = obj_node->next;
		}
		ok = 1;
	}

    /* Set property */
	val.s = buf;
	SetObjectProperty( obj, COMMON_NAME, &val, TWC_TRUE, TWC_TRUE);
	return 1;
}

/**
 * Style filter, not used for preview.
 */
int StyleFilter( TWC_OBJECT *obj) /* object */
{
	DWORD style;

	style = obj->style;
	style = style & ~(WS_DISABLED) | WS_VISIBLE;

	return style;
}

/**
 * Check if new property value is correct.
 */
int PropertyFilter( TWC_OBJECT *obj, /* object */
                    int prop_id,    /* property ID */
                    VALUE *val)     /* value */
{
    /* Name check */
	if ( prop_id == COMMON_NAME ) {
        OBJ_LIST_ITERATE_BEGIN( twc_GetParentChildList( obj));
			if ( node->elem->name && _tcscmp( node->elem->name, val->s) == 0 && node->elem != obj ) {
				MessageBox( hMainWnd, T("Control with this name already exists!"), T("Error!"), 0);
				return PF_WRONG;
			}
        OBJ_LIST_ITERATE_END();
	}

	if ( (prop_id >= COMMON_PROPERTIES_BEGIN) && (prop_id < COMMON_PROPERTIES_END) ) {
        /* Common properties */
		switch ( prop_id ) {
			case COMMON_NAME:
				if ( val->s[0] == T('\0') ) {
					return PF_WRONG;
				}
				break;
			case COMMON_WIDTH:
				if ( val->i < 2 ) {
					val->i = 2;
					return PF_CORRECTED;
				}
				break;
			case COMMON_HEIGHT:
				if ( val->i < 2 ) {
					val->i = 2;
					return PF_CORRECTED;
				}
				break;
			case COMMON_BORDER:
				if ( obj->id == CTRL_ID_WINDOW ) return PF_WRONG;
				break;
			case COMMON_CUSTOMSTYLE:
				if ( val->s[0] == T('\0') ) {
					return PF_DEFAULT;
				}
				break;
		}
	} else if ( obj->id == -1 ) {
        /* Window properties */
		switch ( prop_id ) {
			case WINDOW_CLASSNAME:
				break;
		}
	} else {
        /* Control properties */
		switch ( obj->id ) {
			case CTRL_ID_COMBOBOX:
				if ( obj->style & CBS_SIMPLE && obj->style & CBS_DROPDOWN ) {
					switch ( prop_id ) {
						case COMBOBOX_LOWERCASE:
						case COMBOBOX_UPPERCASE:
							return PF_WRONG;
					}
				}
				break;
		}
	}

	return PF_OK;
}