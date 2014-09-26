#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <windows.h>

#include "twc_design.h"

int AddToolboxButtons( HWND hwnd);
BOOL IsToolboxControlSelected();
UINT GetCurrentToolboxControlId();
int ProcessToolboxElemSelection( UINT tb_id);
int ReleaseToolboxElemSelection();

#endif