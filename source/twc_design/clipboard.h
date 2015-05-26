#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include "object.h"

DLIST_PTWC_OBJECT *GetClipboard();
int ClearClipboard();

int AddObjectToClipboard( TWC_OBJECT *obj);
int AddObjectListToClipboard( DLIST_PTWC_OBJECT *obj_list);

int PasteObjectsFromClipboard( TWC_OBJECT *parent);

#endif
