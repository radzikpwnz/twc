#ifndef PROPLST_H
#define PROPLST_H

#include <windows.h>

#include "..\proplist\proplist.h"

#include "object.h"

int LoadPropertyList( HWND hPropList, RT_OBJECT *obj);
int UpdateSingleListProperty( HWND hPropList, RT_OBJECT *obj, UINT prop_id, const VALUE *val);
int OnChangeListProperty( HWND hPropList, RT_OBJECT *obj, UINT prop_id);
int ProcessChangedProperties();

#endif