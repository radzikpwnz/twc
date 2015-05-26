#include <windows.h>
#include <tchar.h>

/**
 * Set one string to another.
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
 * Copy string and return pointer to destination's end (terminating 0).
 */
TCHAR *_mytcscpy( TCHAR *dest, const TCHAR *source)
{
	while( *dest++ = *source++ );
	return dest - 1;
}