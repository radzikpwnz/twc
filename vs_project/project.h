#ifndef PROJECT_H
#define PROJECT_H

#include "object.h"

/* Project information */
typedef struct tagTWCD_PROJECT {
    RT_OBJECT *root_object;  /* pointer to root object */

    struct {
        int maj, min;
    } version;               /* version */
    TCHAR *path;             /* path to project file */
} TWCD_PROJECT, *PTWCD_PROJECT;

/* Current project info */
extern TWCD_PROJECT *cur_project;

/**
 * Get parent object child list.
 */
DLIST_PRT_OBJECT *GetParentChildList( RT_OBJECT *obj);

int LoadCurrentProject();

void UnloadCurrentProject();

#endif
