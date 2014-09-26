#ifndef CONTROL_ACT_H
#define CONTROL_ACT_H

#include "twc_design.h"

int DeleteSelectedObjects();
int CopySelectedObjects();

int MoveSelectedObjects( int dx, int dy);
int MoveSelectedObjectsKey( UINT dir);
int StopMovingObjectsKey();

#endif
