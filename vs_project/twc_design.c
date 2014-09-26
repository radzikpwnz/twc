#include <windows.h>

#include "twc_design.h"

#include "interface.h"


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
