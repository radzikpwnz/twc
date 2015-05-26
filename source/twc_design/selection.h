#ifndef SELECTION_H
#define SELECTION_H

#include "object.h"

RECT *GetSelectionFrameRect();
DLIST_PTWC_OBJECT *GetSelectedObjects();
int ClearSelection();

int AddObjectToSelection( TWC_OBJECT *obj);
int DeleteObjectFromSelection( TWC_OBJECT *obj);

TWC_BOOL IsSelectionActive();

int StartSelection( TWC_OBJECT *parent, int x, int y, TWC_BOOL new);
int ProcessSelectionFrameChange( int new_x, int new_y);
void StopSelection();

#endif