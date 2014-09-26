#define _CRT_NON_CONFORMING_SWPRINTFS

#include <windows.h>

#include "twc_design.h"
#include "proplst.h"

#include "properties.h"


/* Objects name usage count for each object id (used to generate unique name) */
UINT control_names_usage[CONTROL_COUNT];

/**
 * Generate unique object name.
 */
int GenerateObjectName( RT_OBJECT *obj) /* object */
{
	VALUE val;
	TCHAR buf[64];
	DLIST_NODE_PRT_OBJECT *obj_node;
	int ok;
    TCHAR *default_name;

    /* Get default object name */
    default_name = GetControlDefaultObjectName( obj->ctrl_id);
	ok = 0;

    /* Add number after default name until it become unique */
next:
	while (!ok) {
		_stprintf( buf, T("%s%d"), default_name, ++control_names_usage[obj->ctrl_id]);

        obj_node = GetParentChildList( obj)->first;
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
int StyleFilter( RT_OBJECT *obj) /* object */
{
	DWORD style;

	style = obj->style;
	style = style & ~(WS_DISABLED) | WS_VISIBLE;

	return style;
}

/* Destroy and create object window */
int ReCreateObject( RT_OBJECT *obj) /* object */
{
	HWND insert_after;
    int set_current_obj = 0;

	if ( obj->hwnd == 0 ) return 0;

	insert_after = GetNextWindow( obj->hwnd, GW_HWNDPREV);

	DestroyObjectWindow( obj, TWC_FALSE);
    CreateObjectWindow( obj, TWC_FALSE);
    //TODO: insert in zorder
	return 1;
}

/**
 * Apply property changes to object.
 */
int ApplyObjectProperty( RT_OBJECT *obj,    /* object */
                         UINT on_apply_act) /* action(s) */
{
    RECT rect;

    if ( on_apply_act == PROPERTY_ON_APPLY_NONE ) {
        return 1;
    }

    TWC_CHECKIT( obj->hwnd != NULL );

    /* If recreate object, nothing more to do */
    if ( on_apply_act & PROPERTY_ON_APPLY_RECREATE ) {
        ReCreateObject( obj);
        return 1;
    }

    /* Custom actions */
    if ( on_apply_act & PROPERTY_ON_APPLY_CUSTOM_MASK ) {
        switch ( (on_apply_act & PROPERTY_ON_APPLY_CUSTOM_MASK) >> PROPERTY_ON_APPLY_CUSTOM_SHIFT ) {
            /* Set position */
            case PROPERTY_ON_APPLY_CUSTOM_SETPOS:
                SetWindowPos( obj->hwnd, NULL, obj->x, obj->y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
                break;
            /* Set size */
            case PROPERTY_ON_APPLY_CUSTOM_SETSIZE:
                rect.left = rect.top = 0;
                rect.right = obj->width;
                rect.bottom = obj->height;
                if ( obj->flags & OBJ_FLAG_CLIENTSIZE ) {
                    AdjustWindowRectEx( &rect, obj->style, FALSE, obj->exstyle);
                }
                SetWindowPos( obj->hwnd, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);
                break;
            /* Set title */
            case PROPERTY_ON_APPLY_CUSTOM_SETTITLE:
                SetWindowText( obj->hwnd, obj->title);
                break;
            default:
                TWC_CHECKIT( 0 );
        }
    }

    /* Set style */
    if ( on_apply_act & PROPERTY_ON_APPLY_SETSTYLE ) {
        SetWindowLong(obj->hwnd, GWL_STYLE, StyleFilter(obj));
        SetWindowPos(obj->hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
    }

    /* Set extended style */
    if ( on_apply_act & PROPERTY_ON_APPLY_SETEXSTYLE ) {
        SetWindowLong(obj->hwnd, GWL_EXSTYLE, obj->exstyle);
        SetWindowPos(obj->hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
    }

    /* Redraw */
    if ( on_apply_act & PROPERTY_ON_APPLY_REDRAW ) {
        InvalidateRect(obj->hwnd, NULL, 0);
        UpdateWindow(obj->hwnd);
    }

    return 1;
}

#define UPDATE_STYLE(x) {if (prop->val.i) obj->style |= (x); else obj->style &= ~(x);}
#define UPDATE_EXSTYLE(x) {if (prop->val.i) obj->exstyle |= (x); else obj->exstyle &= ~(x);}

/**
 * Update object property.
 *
 * For internal usage.
 */
static int UpdateObjectPropertyInternal( RT_OBJECT *obj, /* object */
                                         UINT prop_id,   /* property ID */
                                         TWC_BOOL apply) /* apply property? */
{
    PROP_ON_SET_ACTION *act;
    PROPERTY_INFO *propinfo;
    PROPERTY *prop;

    /* Get property info and property */
    propinfo = GetPropertyInfo( obj->ctrl_id, prop_id);
	act = &(propinfo->on_set_act);
    prop = GetObjectProperty( obj, prop_id);

    obj->style &= ~(act->style_to_remove);
    obj->style &= ~(act->exstyle_to_remove);

    /* Updating style and extended style */
    UPDATE_STYLE( act->style_to_set);
    UPDATE_EXSTYLE( act->exstyle_to_set);

    if ( act->style_list_to_set ) {
        obj->style |= act->style_list_to_set[prop->val.i];
    }
    if ( act->exstyle_list_to_set ) {
        obj->exstyle |= act->exstyle_list_to_set[prop->val.i];
    }

    /* Custom action */
    if ( act->custom_act_func ) {
        act->custom_act_func( obj, prop_id);
    }

    /* Apply property if necessary */
    if ( apply ) {
        ApplyObjectProperty( obj, propinfo->on_apply_act);
    }

	return 1;
}

/**
 * Update object property (or all properties).
 */
int UpdateObjectProperty( RT_OBJECT *obj, /* object */
                          UINT prop_id,   /* property ID */
                          TWC_BOOL apply)     /* apply property? */
{
    int prop_count;
    int i;

    TWC_CHECKIT( obj != NULL );
    TWC_CHECKIT( prop_id != PROPERTY_UNDEFINED );

    if ( prop_id == PROPERTIES_ALL ) {
        prop_count = GetControlPropertiesCount( obj->ctrl_id);
        for ( i = 0; i < prop_count; i++ ) {
            UpdateObjectPropertyInternal( obj, i, TWC_FALSE);
        }
    } else {
        UpdateObjectPropertyInternal( obj, prop_id, apply);
    }

	return 1;
}

/**
 * Check if new property value is correct.
 */
int PropertyFilter( RT_OBJECT *obj, /* object */
                    int prop_id,    /* property ID */
                    VALUE *val)     /* value */
{
    /* Name check */
	if ( prop_id == COMMON_NAME ) {
        OBJ_LIST_ITERATE_BEGIN( GetParentChildList( obj));
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
				if ( obj->ctrl_id == CTRL_ID_WINDOW ) return PF_WRONG;
				break;
			case COMMON_CUSTOMSTYLE:
				if ( val->s[0] == T('\0') ) {
					return PF_DEFAULT;
				}
				break;
		}
	} else if ( obj->ctrl_id == -1 ) {
        /* Window properties */
		switch ( prop_id ) {
			case WINDOW_CLASSNAME:
				break;
		}
	} else {
        /* Control properties */
		switch ( obj->ctrl_id ) {
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

/**
 * Compare to property values.
 */
static TWC_BOOL IsValuesEQ( const VALUE *val1, /* value 1 */
                            const VALUE *val2, /* value 2 */
                            UINT type)         /* type */
{
    switch (type) {
		case T_INT:
		case T_LIST:
		case T_BOOL:
            if ( val1->i == val2->i ) {
                return TWC_TRUE;
            }
            break;
		case T_STR:
            if ( val1->s == NULL && val2->s == NULL ) {
                return TWC_TRUE;
            }
            if ( (val1->s == NULL && val2->s != NULL)
                 || (val1->s != NULL && val2->s == NULL) ) {
                break;
            }
            if ( _tcscmp( val1->s, val2->s) == 0 ) {
                return TWC_TRUE;
            }
			break;
	}

    return TWC_FALSE;
}

/**
 * Get object property.
 */
PROPERTY *GetObjectProperty( RT_OBJECT *obj, /* object */
                             UINT prop_id)   /* property ID */
{
    TWC_CHECKIT( prop_id != PROPERTIES_ALL);
    TWC_CHECKIT( prop_id < GetControlPropertiesCount( obj->ctrl_id) );

	return &obj->properties[prop_id];
}

/**
 * Get object property value.
 */
VALUE *GetObjectPropertyVal( RT_OBJECT *obj, /* object */
                             int prop_id)    /* property ID */
{
    return &GetObjectProperty(obj, prop_id)->val;
}

/**
 * Get object property flags.
 */
UINT GetObjectPropertyFlags( RT_OBJECT *obj, /* object */
                             int prop_id)    /* property ID */
{
    return GetObjectProperty(obj, prop_id)->flags;
}

/**
 * Set object property integer value.
 */
int SetObjectPropertyInt( RT_OBJECT *obj, /* object */
                          UINT prop_id,   /* property ID */
                          int val,        /* new value */
                          TWC_BOOL update,    /* update property? */
                          TWC_BOOL apply)     /* apply property? */
{
    VALUE value;

    value.i = val;
    return SetObjectProperty( obj, prop_id, &value, update, apply);
}

/**
 * Set object property string value.
 */
int SetObjectPropertyStr( RT_OBJECT *obj,   /* object */
                          UINT prop_id,     /* property ID */
                          const TCHAR *val, /* new value */
                          TWC_BOOL update,      /* update property? */
                          TWC_BOOL apply)       /* apply property? */
{
    VALUE value;

    value.s = val;
    return SetObjectProperty( obj, prop_id, &value, update, apply);
}

/**
 * Set object property value.
 */
int SetObjectProperty( RT_OBJECT *obj,       /* object */
                       UINT prop_id,         /* property ID */
                       const VALUE *new_val, /* new value */
                       TWC_BOOL update,          /* update property? */
                       TWC_BOOL apply)           /* apply property? */
{
	PROPERTY_INFO *propinfo;
	PROPERTY *prop;

	propinfo = GetPropertyInfo(obj->ctrl_id, prop_id);
    prop = GetObjectProperty( obj, prop_id);

    TWC_CHECKIT( !(propinfo->type == T_STR && new_val->s == NULL) );

    prop->flags |= PROPERTY_FLAG_SET;

    /* Default value check */
	if ( IsValuesEQ( new_val, &propinfo->default_caption, propinfo->type) ) {
        prop->flags |= PROPERTY_FLAG_DEFAULT;
    } else {
        prop->flags &= ~PROPERTY_FLAG_DEFAULT;
    }

    /* Same value check */
    if ( IsValuesEQ( new_val, &prop->val, propinfo->type) ) {
        return 1;
    }

    /* Set new value */
	switch (propinfo->type) {
		case T_INT:
		case T_LIST:
		case T_BOOL:
			prop->val.i = new_val->i;
			break;
		case T_STR:
			SetString( &(prop->val.s), new_val->s, 1);
			break;
	}

    /* Update property */
    if ( update ) {
        UpdateObjectProperty( obj, prop_id, apply);
    }

    /* If object is current, update property in property list */
    if ( obj == current_object ) {
        TWC_CHECKIT( obj->hwnd != NULL );
        UpdateSingleListProperty( hPropList, obj, prop_id, &(prop->val));
    }

	return 1;
}

/**
 * Set default value to property or all properties.
 */
int SetObjectPropertyDefaultValue( RT_OBJECT *obj, /* object */
                                   UINT prop_id)   /* property ID */
{
    int prop_count;
    int i;
    PROPERTY_INFO *propinfo;

    TWC_CHECKIT( obj != NULL );

    if ( prop_id == PROPERTIES_ALL ) {
        prop_count = GetControlPropertiesCount( obj->ctrl_id);
        for ( i = 0; i < prop_count; i++ ) {
            propinfo = GetPropertyInfo( obj->ctrl_id, i);
            SetObjectProperty( obj, i, &(propinfo->default_caption), TWC_TRUE, TWC_FALSE);
        }
    } else {
        propinfo = GetPropertyInfo( obj->ctrl_id, prop_id);
        SetObjectProperty( obj, prop_id, &(propinfo->default_caption), TWC_TRUE, TWC_FALSE);
    }

    return 1;
}
