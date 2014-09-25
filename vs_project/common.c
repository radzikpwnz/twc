#include <windows.h>

#include "common.h"

#include "interface.h"

TCHAR program_version_str[] = T("1.0 beta 5");

#undef free

void my_free(void **p)
{
	if (*p) free(*p);
	*p = NULL;
}

#define free(ptr) my_free(&(ptr))

int SetString(TCHAR **str, TCHAR *new_str, int copy)
{
	if (*str != new_str) free(*str);

	if (new_str == NULL) {
		*str = NULL;
		return 1;
	}

	if (copy) {
		*str = malloc((_tcslen(new_str) + 1) * sizeof(TCHAR));
		_tcscpy(*str, new_str);
	} else {
		*str = new_str;
	}
	return 1;
}

void SetStatusText(TCHAR *str)
{
	SendMessage(StatusBar.hwnd, SB_SETTEXT, 0, (LPARAM)str);
	return;
}

TCHAR *_mytcscpy(TCHAR *dest, const TCHAR *source)
{
	while(*dest++ = *source++);
	return dest - 1;
}

const TCHAR *GetProgramVersion()
{
    return program_version_str;
}

void twc_Fatal( TCHAR *file, int line)
{
    TCHAR buf[256];

    _stprintf( buf, T("Assertion failed in file \"%s\", line %d. Program will be terminated!"), file, line);
    MessageBox( hMainWnd, buf, T("Error!"), 0);
    ExitProcess(1);
    return;
}