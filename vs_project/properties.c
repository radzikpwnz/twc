#define _CRT_NON_CONFORMING_SWPRINTFS

#include <windows.h>
#include <tchar.h>

#include "common.h"
#include "proplst.h"

#include "properties.h"

UINT control_names_usage[CONTROL_COUNT];


int GenerateObjectName(RT_OBJECT *obj)
{
	VALUE val;
	TCHAR buf[64];
	DLIST_NODE_PRT_OBJECT *obj_node;
	int ok;
    TCHAR *default_name;

    default_name = GetControlDefaultObjectName( obj->ctrl_id);
	ok = 0;
next:
	while (!ok) {
		_stprintf(buf, T("%s%d"), default_name, ++control_names_usage[obj->ctrl_id]);

        obj_node = GetParentChildList( obj)->first;
		while (obj_node != NULL) {
			if (obj_node->elem->name && _tcscmp(obj_node->elem->name, buf) == 0 && obj_node->elem != obj) {
				ok = 0;
				goto next;
			}
			ok = 1;
			obj_node = obj_node->next;
		}
		ok = 1;
	}

	val.s = buf;
	SetObjectProperty(obj, COMMON_NAME, &val, TWC_TRUE, TWC_TRUE);
	return 1;
}

int StyleFilter(RT_OBJECT *obj)
{
	DWORD style;

	style = obj->style;
	style = style & ~(WS_DISABLED) | WS_VISIBLE;

	return style;
}

int ReCreateObject(RT_OBJECT *obj)
{
	HWND insert_after;
    int set_current_obj = 0;

	if (obj->hwnd == 0) return 0;

	insert_after = GetNextWindow( obj->hwnd, GW_HWNDPREV);

	DestroyObjectWindow( obj, TWC_FALSE);
    CreateObjectWindow( obj, TWC_FALSE);
    //TODO: insert in zorder
	return 1;
}

int ApplyObjectProperty( RT_OBJECT *obj, UINT on_apply_act)
{
    RECT rect;

    if ( on_apply_act == PROPERTY_ON_APPLY_NONE ) {
        return 1;
    }

    TWC_CHECKIT( obj->hwnd != NULL );

    if ( on_apply_act & PROPERTY_ON_APPLY_RECREATE ) {
        ReCreateObject( obj);
        return 1;
    }

    if ( on_apply_act & PROPERTY_ON_APPLY_CUSTOM_MASK ) {
        switch ( (on_apply_act & PROPERTY_ON_APPLY_CUSTOM_MASK) >> PROPERTY_ON_APPLY_CUSTOM_SHIFT ) {
            case PROPERTY_ON_APPLY_CUSTOM_SETPOS:
                SetWindowPos( obj->hwnd, NULL, obj->x, obj->y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
                break;
            case PROPERTY_ON_APPLY_CUSTOM_SETSIZE:
                rect.left = rect.top = 0;
                rect.right = obj->width;
                rect.bottom = obj->height;
                if ( obj->flags & OBJ_FLAG_CLIENTSIZE ) {
                    AdjustWindowRectEx( &rect, obj->style, FALSE, obj->exstyle);
                }
                SetWindowPos( obj->hwnd, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);
                break;
            case PROPERTY_ON_APPLY_CUSTOM_SETTITLE:
                SetWindowText( obj->hwnd, obj->title);
                break;
            default:
                TWC_CHECKIT( 0 );
        }
    }

    if ( on_apply_act & PROPERTY_ON_APPLY_SETSTYLE ) {
        SetWindowLong(obj->hwnd, GWL_STYLE, StyleFilter(obj));
        SetWindowPos(obj->hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
    }

    if ( on_apply_act & PROPERTY_ON_APPLY_SETEXSTYLE ) {
        SetWindowLong(obj->hwnd, GWL_EXSTYLE, obj->exstyle);
        SetWindowPos(obj->hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
    }

    if ( on_apply_act & PROPERTY_ON_APPLY_REDRAW ) {
        InvalidateRect(obj->hwnd, NULL, 0);
        UpdateWindow(obj->hwnd);
    }

    return 1;
}

#define UPDATE_STYLE(x) {if (prop->val.i) obj->style |= (x); else obj->style &= ~(x);}
#define UPDATE_EXSTYLE(x) {if (prop->val.i) obj->exstyle |= (x); else obj->exstyle &= ~(x);}

static int UpdateObjectPropertyInternal( RT_OBJECT *obj, UINT prop_id, BOOL apply)
{
    PROP_ON_SET_ACTION *act;
    PROPERTY_INFO *propinfo;
    PROPERTY *prop;

    propinfo = GetPropertyInfo( obj->ctrl_id, prop_id);
	act = &(propinfo->on_set_act);
    prop = GetObjectProperty( obj, prop_id);

    obj->style &= ~(act->style_to_remove);
    obj->style &= ~(act->exstyle_to_remove);

    UPDATE_STYLE( act->style_to_set);
    UPDATE_EXSTYLE( act->exstyle_to_set);

    if ( act->style_list_to_set ) {
        obj->style |= act->style_list_to_set[prop->val.i];
    }
    if ( act->exstyle_list_to_set ) {
        obj->exstyle |= act->exstyle_list_to_set[prop->val.i];
    }

    if ( act->custom_act_func ) {
        act->custom_act_func( obj, prop_id);
    }

    if ( apply ) {
        ApplyObjectProperty( obj, propinfo->on_apply_act);
    }

	return 1;
}

int UpdateObjectProperty( RT_OBJECT *obj, UINT prop_id, BOOL apply)
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

int PropertyFilter( RT_OBJECT *obj, int prop_id, VALUE *val)
{
	DLIST_NODE_PRT_OBJECT *obj_node;
	//PROPERTY_INFO *propinfo;

	if (prop_id == COMMON_NAME) {
		(void *)obj_node = (obj->parent == NULL) ? (void *)cur_project.obj_list.first : (void *)obj->parent->child_list.first;
		while (obj_node != NULL) {
			if (obj_node->elem->name && _tcscmp(obj_node->elem->name, val->s) == 0 && obj_node->elem != obj) {
				MessageBox(hMainWnd, T("Control with this name already exists!"), T("Error!"), 0);
				return PF_WRONG;
			}
			obj_node = obj_node->next;
		}
	}

	if ((prop_id >= COMMON_PROPERTIES_BEGIN) && (prop_id < COMMON_PROPERTIES_END)) { //Common properties
		switch (prop_id) {
			case COMMON_NAME:
				if (val->s[0] == T('\0')) {
					return PF_WRONG;
				}
				break;
			case COMMON_WIDTH:
				if (val->i < 2) {
					val->i = 2;
					return PF_CORRECTED;
				}
				break;
			case COMMON_HEIGHT:
				if (val->i < 2) {
					val->i = 2;
					return PF_CORRECTED;
				}
				break;
			case COMMON_BORDER:
				if (obj->ctrl_id == - 1) return PF_WRONG;
				break;
			case COMMON_DISABLED:
				break;
			case COMMON_CUSTOMSTYLE:
				if (val->s[0] == T('\0')) {
					return PF_DEFAULT;
				}
				break;
		}
	} else if (obj->ctrl_id == -1) { //Window properties
		switch (prop_id) {
			case WINDOW_CLASSNAME:
				break;
		}
	} else { //Control properties
		switch (obj->ctrl_id) {
			case CTRL_ID_COMBOBOX:
				if (obj->style & CBS_SIMPLE && obj->style & CBS_DROPDOWN) {
					switch (prop_id) {
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

BOOL IsValuesEQ( const VALUE *val1, const VALUE *val2, UINT type)
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

PROPERTY *GetObjectProperty(RT_OBJECT *obj, UINT prop_id)
{
    TWC_CHECKIT( prop_id != PROPERTIES_ALL);
    TWC_CHECKIT( prop_id < GetControlPropertiesCount( obj->ctrl_id) );

	return &obj->properties[prop_id];
}

VALUE *GetObjectPropertyVal(RT_OBJECT *obj, int prop_id)
{
    return &GetObjectProperty(obj, prop_id)->val;
}

UINT GetObjectPropertyFlags(RT_OBJECT *obj, int prop_id)
{
    return GetObjectProperty(obj, prop_id)->flags;
}

int SetObjectPropertyInt( RT_OBJECT *obj, UINT prop_id, int val, BOOL update, BOOL apply)
{
    VALUE value;

    value.i = val;
    return SetObjectProperty( obj, prop_id, &value, update, apply);
}

int SetObjectPropertyStr( RT_OBJECT *obj, UINT prop_id, TCHAR *val, BOOL update, BOOL apply)
{
    VALUE value;

    value.s = val;
    return SetObjectProperty( obj, prop_id, &value, update, apply);
}

int SetObjectProperty(RT_OBJECT *obj, UINT prop_id, const VALUE *new_val, BOOL update, BOOL apply)
{
	PROPERTY_INFO *propinfo;
	PROPERTY *prop;

	propinfo = GetPropertyInfo(obj->ctrl_id, prop_id);
    prop = GetObjectProperty( obj, prop_id);

    TWC_CHECKIT( !(propinfo->type == T_STR && new_val->s == NULL) );

    prop->flags |= PROPERTY_FLAG_SET;

    //Default value
	if ( IsValuesEQ( new_val, &propinfo->default_caption, propinfo->type) ) {
        prop->flags |= PROPERTY_FLAG_DEFAULT;
    } else {
        prop->flags &= ~PROPERTY_FLAG_DEFAULT;
    }

    //Same value
    if ( IsValuesEQ( new_val, &prop->val, propinfo->type) ) {
        return 1;
    }

    //Set new value
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

    if ( update ) {
        UpdateObjectProperty( obj, prop_id, apply);
    }

    if ( obj == current_object ) {
        TWC_CHECKIT( obj->hwnd != NULL );
        UpdateSingleListProperty( hPropList, obj, prop_id, &(prop->val));
    }

	return 1;
}

int SetObjectPropertyDefaultValue( RT_OBJECT *obj, UINT prop_id)
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