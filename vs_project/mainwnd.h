#ifndef MAINWND_H
#define MAINWND_H

#include <windows.h>

#include "common.h"

LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);

void actGenerateCode();
void actNewProject();
void actOpenProject();
void actSaveProject();
void actPreview();

#endif