#ifndef IL_H
#define IL_H

#include <windows.h>

#include "common.h"

#define TWCIL_HEADER T("TWCIL 1.0\n\n")

int LoadProjectFromFile(TCHAR *path, TWCD_PROJECT *_project, int *err_pos);
int SaveProjectToFile(TWCD_PROJECT *project, TCHAR *path);
//int LoadWindow( TCHAR *path, RT_WINDOW *wnd);


#endif