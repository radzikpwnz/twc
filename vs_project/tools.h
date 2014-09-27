#ifndef TOOLS_H
#define TOOLS_H

#include "twc_design.h"

#include "object.h"

/* Checker */
#ifdef TWC_CHECKERS_ENABLED

#define TWC_CHECKIT(x) { if ( !(x) ) twc_Fatal( T(__FILE__), __LINE__); }

/* Terminate program and show error message */
void twc_Fatal( TCHAR *file, int line);

#else

#define TWC_CHECKIT(x)

#endif

/* Search object in object list */
DLIST_NODE_PRT_OBJECT *FindObjectInList( RT_OBJECT *obj, DLIST_PRT_OBJECT *list);

/* Delete object from object list */
TWC_BOOL RemoveObjectFromList( RT_OBJECT *obj, DLIST_PRT_OBJECT *list);

int PerformActionOnObjectsInList( DLIST_PRT_OBJECT *list, int (*func)(RT_OBJECT *));
int GetObjectRect( RT_OBJECT *obj, RECT *rect);
int RoundByGrid( int x);

/* Frees pointer and writes NULL to it */
#define free( ptr) my_free( &(ptr))
void my_free( void **p);

/* Set one string to another */
int SetString(TCHAR **str, TCHAR *new_str, int copy);

/* Set status text */
void SetStatusText( TCHAR *str);

/* Copy string and return pointer to destination's end (terminating 0) */
TCHAR *_mytcscpy( TCHAR *dest, const TCHAR *source);

/*Get program version string */
const TCHAR *GetProgramVersion();

#endif