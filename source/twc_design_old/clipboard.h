#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include "object.h"

DLIST_PRT_OBJECT *GetClipboard();
int ClearClipboard();

int AddObjectToClipboard( RT_OBJECT *obj);
int AddObjectListToClipboard( DLIST_PRT_OBJECT *obj_list);

int PasteObjectsFromClipboard( RT_OBJECT *parent);

#endif
