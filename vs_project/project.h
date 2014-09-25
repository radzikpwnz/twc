#ifndef PROJECT_H
#define PROJECT_H

#include "object.h"

typedef struct tagTWCD_PROJECT {
    struct {
        int maj, min;
    } version;
    DLIST_PRT_OBJECT obj_list;
    TCHAR *path;
} TWCD_PROJECT, *PTWCD_PROJECT;


extern TWCD_PROJECT cur_project;

int LoadCurrentProject();
void UnloadCurrentProject();

#endif