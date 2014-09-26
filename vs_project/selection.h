#ifndef SELECTION_H
#define SELECTION_H

#include "twc_design.h"

RECT *GetSelectionFrameRect();
DLIST_PRT_OBJECT *GetSelectedObjects();
int ClearSelection();

int AddObjectToSelection( RT_OBJECT *obj);
int DeleteObjectFromSelection( RT_OBJECT *obj);

TWC_BOOL IsSelectionActive();

int StartSelection( RT_OBJECT *parent, int x, int y, TWC_BOOL new);
int ProcessSelectionFrameChange( int new_x, int new_y);
void StopSelection();

#endif