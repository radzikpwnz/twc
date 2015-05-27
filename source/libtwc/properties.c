#include "libtwc.h"

#include "object.h"

#include "properties.h"


/**
 * Apply property changes to object.
 */
static int ApplyObjectProperty( TWC_OBJECT *obj,   /* object */
                                UINT on_apply_act) /* action(s) */
{
    RECT rect;

    if ( on_apply_act == PROPERTY_ON_APPLY_NONE ) {
        return 1;
    }

    TWC_CHECKIT( obj->hwnd != NULL );

    /* If recreate object, nothing more to do */
    if ( on_apply_act & PROPERTY_ON_APPLY_RECREATE ) {
        twc_ReCreateObjectWindow( obj);
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
        SetWindowLong(obj->hwnd, GWL_STYLE, obj->style);
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
static int UpdateObjectPropertyInternal( TWC_OBJECT *obj, /* object */
                                         UINT prop_id,    /* property ID */
                                         TWC_BOOL apply)  /* apply property? */
{
    PROP_ON_SET_ACTION *act;
    PROPERTY_INFO *propinfo;
    PROPERTY *prop;

    /* Get property info and property */
    propinfo = twc_GetPropertyInfo( obj->id, prop_id);
	act = &(propinfo->on_set_act);
    prop = twc_GetObjectProperty( obj, prop_id);

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
static int UpdateObjectProperty( TWC_OBJECT *obj, /* object */
                                 UINT prop_id,    /* property ID */
                                 TWC_BOOL apply)  /* apply property? */
{
    int prop_count;
    int i;

    TWC_CHECKIT( obj != NULL );
    TWC_CHECKIT( prop_id != PROPERTY_UNDEFINED );

    if ( prop_id == PROPERTIES_ALL ) {
        prop_count = twc_GetControlPropertiesCount( obj->id);
        for ( i = 0; i < prop_count; i++ ) {
            UpdateObjectPropertyInternal( obj, i, TWC_FALSE);
        }
    } else {
        UpdateObjectPropertyInternal( obj, prop_id, apply);
    }

	return 1;
}

/**
 * Compare two property values.
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
PROPERTY *twc_GetObjectProperty( TWC_OBJECT *obj, /* object */
                                 UINT prop_id)    /* property ID */
{
    TWC_CHECKIT( prop_id != PROPERTIES_ALL);
    TWC_CHECKIT( prop_id < twc_GetControlPropertiesCount( obj->id) );

	return &obj->properties[prop_id];
}

/**
 * Get object property value.
 */
VALUE *twc_GetObjectPropertyVal( TWC_OBJECT *obj, /* object */
                                 int prop_id)     /* property ID */
{
    return &twc_GetObjectProperty(obj, prop_id)->val;
}

/**
 * Get object property flags.
 */
UINT twc_GetObjectPropertyFlags( TWC_OBJECT *obj, /* object */
                                 int prop_id)     /* property ID */
{
    return twc_GetObjectProperty(obj, prop_id)->flags;
}

/**
 * Set object property integer value.
 */
int twc_SetObjectPropertyInt( TWC_OBJECT *obj, /* object */
                              UINT prop_id,    /* property ID */
                              int val,         /* new value */
                              TWC_BOOL update, /* update property? */
                              TWC_BOOL apply)  /* apply property? */
{
    VALUE value;

    value.i = val;
    return twc_SetObjectProperty( obj, prop_id, &value, update, apply);
}

/**
 * Set object property string value.
 */
int twc_SetObjectPropertyStr( TWC_OBJECT *obj,  /* object */
                              UINT prop_id,     /* property ID */
                              const TCHAR *val, /* new value */
                              TWC_BOOL update,  /* update property? */
                              TWC_BOOL apply)   /* apply property? */
{
    VALUE value;

    value.s = (TCHAR *)val;
    return twc_SetObjectProperty( obj, prop_id, &value, update, apply);
}

/**
 * Set object property value.
 */
int twc_SetObjectProperty( TWC_OBJECT *obj,      /* object */
                           UINT prop_id,         /* property ID */
                           const VALUE *new_val, /* new value */
                           TWC_BOOL update,      /* update property? */
                           TWC_BOOL apply)       /* apply property? */
{
	PROPERTY_INFO *propinfo;
	PROPERTY *prop;

	propinfo = twc_GetPropertyInfo(obj->id, prop_id);
    prop = twc_GetObjectProperty( obj, prop_id);

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

	return 1;
}

/**
 * Set default value to property or all properties.
 */
int twc_SetObjectPropertyDefaultValue( TWC_OBJECT *obj, /* object */
                                       UINT prop_id)    /* property ID */
{
    int prop_count;
    int i;
    PROPERTY_INFO *propinfo;

    TWC_CHECKIT( obj != NULL );

    if ( prop_id == PROPERTIES_ALL ) {
        prop_count = twc_GetControlPropertiesCount( obj->id);
        for ( i = 0; i < prop_count; i++ ) {
            propinfo = twc_GetPropertyInfo( obj->id, i);
            twc_SetObjectProperty( obj, i, &(propinfo->default_caption), TWC_TRUE, TWC_FALSE);
        }
    } else {
        propinfo = twc_GetPropertyInfo( obj->id, prop_id);
        twc_SetObjectProperty( obj, prop_id, &(propinfo->default_caption), TWC_TRUE, TWC_FALSE);
    }

    return 1;
}
