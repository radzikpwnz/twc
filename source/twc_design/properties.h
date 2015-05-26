#ifndef PROPERTIES_H
#define PROPERTIES_H

#include "twc_design.h"

/* Codes returned by PropertyFilter */
enum tagPROPFILTER_CODE {
	PF_WRONG,     /* wrong value */
	PF_CORRECTED, /* wrong value, but it has been corrected */
	PF_DEFAULT,   /* value default value for this property */
	PF_OK         /* value ok */
} PROPFILTER_CODE;

int GenerateObjectName( TWC_OBJECT *obj);
int StyleFilter( TWC_OBJECT *obj);
int PropertyFilter( TWC_OBJECT *obj, int prop_id, VALUE *val);

#endif