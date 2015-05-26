#ifndef WIN_STUFF_H
#define WIN_STUFF_H

int MyGetOpenFileName(HWND hwnd, TCHAR *path, int bufsize, const TCHAR *filter, const TCHAR *title);
int MyGetSaveFileName(HWND hwnd, TCHAR *path, int bufsize, const TCHAR *def_ext, const TCHAR *filter, const TCHAR *title);
void SetCurrentDirToExeFolder();
int GetFileVersion(TCHAR *file, DWORD *ms, DWORD *ls);
HBITMAP MakeBitMapTransparent(HBITMAP hbmSrc);

#endif