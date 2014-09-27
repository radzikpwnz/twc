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
 * Search object in object list.
 *
 * Returns:
 *   pointer to list node containing object, if found
 *   NULL, otherwise
 */
DLIST_NODE_PRT_OBJECT *FindObjectInList( RT_OBJECT *obj,         /* object to find */
                                         DLIST_PRT_OBJECT *list) /* object list */
{
    OBJ_LIST_ITERATE_BEGIN( list);
        if ( node->elem == obj ) {
            return node;
        }
    OBJ_LIST_ITERATE_END();

    return NULL;
}

/**
 * Delete object from object list.
 *
 * Returns:
 *   TWC_TRUE, if object found and deleted
 *   TWC_FALSE, otherwise
 */
TWC_BOOL RemoveObjectFromList( RT_OBJECT *obj,         /* object to remove */
                               DLIST_PRT_OBJECT *list) /* object list */
{
    DLIST_NODE_PRT_OBJECT *node;

    node = FindObjectInList( obj, list); 
    if ( node != NULL ) {
        DListRemove( list, node);
        return 1;
    }

    return 0;
}

/**
 * Call some function for each object in list as argument.
 */
int PerformActionOnObjectsInList( DLIST_PRT_OBJECT *list,   /* object list */
                                  int (*func)(RT_OBJECT *)) /* function */
{
    OBJ_LIST_ITERATE_BEGIN( list);
        if ( func( node->elem) == 0 ) {
            return 0;
        }
    OBJ_LIST_ITERATE_END();

    return 1;
}

/**
 * Get object position and size to RECT structure.
 */
int GetObjectRect( RT_OBJECT *obj, /* object */
                   RECT *rect)     /* (out) pointer to RECT structure */
{
    rect->left = obj->x;
    rect->right = obj->x + obj->width;
    rect->top = obj->y;
    rect->bottom = obj->y + obj->height;

    return 1;
}

/**
 * Round value by current grid size.
 */
int RoundByGrid( int x) /* value */
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