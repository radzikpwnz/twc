#include <windows.h>
#include <tchar.h>

#include "win_stuff.h"

typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)( int );

int MyGetOpenFileName(HWND hwnd, TCHAR *path, int bufsize, const TCHAR *filter, const TCHAR *title)
{
  OPENFILENAME ofn;
  RtlSecureZeroMemory(&ofn, sizeof(OPENFILENAME));
  ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = path;
	ofn.nMaxFile = bufsize;
  ofn.lpstrTitle = title;
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (GetOpenFileName(&ofn) == 0) {
		if (CommDlgExtendedError() != 0) {
		  return -1;
		}
		return 0;
	}
  return 1;
}

int MyGetSaveFileName(HWND hwnd, TCHAR *path, int bufsize, const TCHAR *def_ext, const TCHAR *filter, const TCHAR *title)
{
  OPENFILENAME ofn;
  RtlSecureZeroMemory(&ofn,sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = path;
	ofn.nMaxFile = bufsize;
  ofn.lpstrDefExt = def_ext;
  ofn.lpstrFilter = filter;
  ofn.lpstrTitle = title;
  ofn.nFilterIndex = 1;
  ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
  if (GetSaveFileName(&ofn) == 0) {
		if (CommDlgExtendedError() != 0) {
      return -1;
		}
    return 0;
	}
  return 1;
}

void SetCurrentDirToExeFolder()
{
	TCHAR exe_path[MAX_PATH + 1];

	GetModuleFileName(NULL, exe_path, MAX_PATH);
	*(_tcsrchr(exe_path, '\\') + 1) = '\0';
	SetCurrentDirectory(exe_path);
	return;
}

int GetFileVersion(TCHAR *file, DWORD *ms, DWORD *ls)
{
	int len;
	TCHAR *buf;
	VS_FIXEDFILEINFO *fileinfo;

	len = GetFileVersionInfoSize(file , NULL);
	if (!len) {
		*ms = *ls = 0;
		return 0;
	}
	__asm {
		sub esp, len
		mov buf, esp
	}
	//buf = malloc(len);
	GetFileVersionInfo(file, 0, len, buf);
	VerQueryValue(buf, TEXT("\\"), &fileinfo, NULL);
	*ms = fileinfo->dwFileVersionMS;
	*ls = fileinfo->dwFileVersionLS;
	__asm {
		add esp, len
	}
	//free(buf);
	return 1;
}

HBITMAP MakeBitMapTransparent(HBITMAP hbmSrc)
{
	HDC hdcSrc, hdcDst;
	HBITMAP hbmNew;
	BITMAP bm;
	COLORREF clrTP, clrBK;
	int nRow, nCol;

	if ((hdcSrc = CreateCompatibleDC(NULL)) != NULL) {
		if ((hdcDst = CreateCompatibleDC(NULL)) != NULL) {
			GetObject(hbmSrc, sizeof(bm), &bm);

			SelectObject(hdcSrc, hbmSrc);
			hbmNew = CreateBitmap(bm.bmWidth, bm.bmHeight, bm.bmPlanes, bm.bmBitsPixel, NULL);
			SelectObject(hdcDst, hbmNew);
			
			clrTP = RGB(255, 0, 255);
			clrBK = GetSysColor(COLOR_BTNFACE);

			BitBlt(hdcDst, 0, 0, bm.bmWidth, bm.bmHeight, hdcSrc, 0, 0, SRCCOPY);
			
			for (nRow = 0; nRow < bm.bmHeight; nRow++)// work our way through all the pixels changing their color
				for (nCol = 0; nCol < bm.bmWidth; nCol++)// when we hit our set transparency color.
					if (GetPixel(hdcDst, nCol, nRow) == clrTP)
						SetPixel(hdcDst, nCol, nRow, clrBK);
			
			DeleteDC(hdcDst);
		}
		DeleteDC(hdcSrc);
		
	}
	return hbmNew;// return our transformed bitmap.
}