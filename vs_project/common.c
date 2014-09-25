#include <windows.h>

#include "common.h"

#include "interface.h"

/* Program version */
TCHAR program_version_str[] = T("1.0 beta 5");


#undef free

/**
 * Frees pointer and writes NULL to it
 */
void my_free(void **p)
{
	if (*p) free(*p);
	*p = NULL;
}

#define free(ptr) my_free(&(ptr))


/**
 * Set one string to another.
 *
 * If <copy> is TRUE, copies new_str to str.
 */
int SetString( TCHAR **str, TCHAR *new_str, int copy)
{
	if ( *str != new_str ) free(*str);

	if ( new_str == NULL ) {
		*str = NULL;
		return 1;
	}

	if ( copy ) {
		*str = malloc( (_tcslen(new_str) + 1) * sizeof(TCHAR));
		_tcscpy( *str, new_str);
	} else {
		*str = new_str;
	}
	return 1;
}

/**
 * Set status text.
 */
void SetStatusText( TCHAR *str)
{
	SendMessage( StatusBar.hwnd, SB_SETTEXT, 0, (LPARAM)str);
	return;
}

/**
 * Copy string and return pointer to destination's end (terminating 0).
 */
TCHAR *_mytcscpy( TCHAR *dest, const TCHAR *source)
{
	while( *dest++ = *source++ );
	return dest - 1;
}

/**
 * Get program version string.
 */
const TCHAR *GetProgramVersion()
{
    return program_version_str;
}

/**
 * Terminate program and show error message.
 */
void twc_Fatal( TCHAR *file, int line)
{
    TCHAR buf[256];

    _stprintf( buf, T("Assertion failed in file \"%s\", line %d. Program will be terminated!"), file, line);
    MessageBox( hMainWnd, buf, T("Error!"), 0);
    ExitProcess( 1);
    return;
}

/**
 * Add text to log.
 */
/*void AddLog(const TCHAR *text)
{
	register int n;

	n = GetWindowTextLength(txtLog.hwnd);
	SendMessage(txtLog.hwnd, EM_SETSEL, (WPARAM)n, (LPARAM)n);
	SendMessage(txtLog.hwnd, EM_REPLACESEL, 0, (LPARAM) text);
	return;
}*/
