#ifndef PROJECT_H
#define PROJECT_H

#include "object.h"

/* Project information */
typedef struct tagTWCD_PROJECT {
    struct {
        int maj, min;
    } version;                 /* version */
    DLIST_PRT_OBJECT obj_list; /* object list */
    TCHAR *path;               /* path to project file */
} TWCD_PROJECT, *PTWCD_PROJECT;

/* Current project info */
extern TWCD_PROJECT cur_project;

/**
 * Get parent object child list.
 */
DLIST_PRT_OBJECT *GetParentChildList( RT_OBJECT *obj);

int LoadCurrentProject();

void UnloadCurrentProject();

#endif
