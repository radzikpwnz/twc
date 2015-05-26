#ifndef TOOLS_H
#define TOOLS_H

#include <windows.h>

/* Set one string to another */
int SetString(TCHAR **str, TCHAR *new_str, int copy);

/* Copy string and return pointer to destination's end (terminating 0) */
TCHAR *_mytcscpy( TCHAR *dest, const TCHAR *source);

#endif