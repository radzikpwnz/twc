#include "object.h"

#include "interface.h"

#include "tools.h"


DLIST_NODE_PRT_OBJECT *FindObjectInList( RT_OBJECT *obj, DLIST_PRT_OBJECT *list)
{
    DLIST_NODE_PRT_OBJECT *node;

    node = list->first;
    while ( node != NULL ) {
        if ( node->elem == obj ) {
            return node;
        }
        node = node->next;
    }
    return NULL;
}

int PerformActionOnObjectsInList( DLIST_PRT_OBJECT *list, int (*func)(RT_OBJECT *))
{
    DLIST_NODE_PRT_OBJECT *node;

    node = list->first;
    while ( node != NULL ) {
        if ( func( node->elem) == 0 ) {
            return 0;
        }
        node = node->next;
    }

    return 1;
}

int GetObjectRect( RT_OBJECT *obj, RECT *rect)
{
    rect->left = obj->x;
    rect->right = obj->x + obj->width;
    rect->top = obj->y;
    rect->bottom = obj->y + obj->height;

    return 1;
}

int RoundByGrid( int x)
{
    return x - abs(x) % grid_size;
}

/* Program version */
TCHAR program_version_str[] = T("1.0 beta 5");


#undef free

/**
 * Frees pointer and writes NULL to it.
 */
void my_free(void **p)
{
	if (*p) free(*p);
	*p = NULL;
}

#define free(ptr) my_free(&(ptr))


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