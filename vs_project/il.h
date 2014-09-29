#ifndef IL_H
#define IL_H

#include "project.h"

int LoadProjectFromFile( TCHAR *path, TWCD_PROJECT **project, int *err_pos);
int SaveProjectToFile( TWCD_PROJECT *project, TCHAR *path);

#endif
