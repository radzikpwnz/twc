#ifndef CLIPBOARD_H
#define CLIPBOARD_H

DLIST_PRT_OBJECT *GetCurrentClipboard();
int ClearClipboard();

int PlaceObjectToClipboard( RT_OBJECT *obj);
int PlaceObjectListToClipboard( DLIST_PRT_OBJECT *obj_list);

int PasteObjectsFromClipboard( RT_OBJECT *parent);

#endif
