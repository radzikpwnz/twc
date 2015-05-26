#include <stdio.h>

#include "twc_design.h"

#include "project.h"
#include "win_stuff.h"

/* Code (.c) and header (.h) files headers */
#define CODE_START T( "#include <windows.h>\n" ) \
                   T( "#include \"twc.h\"\n" ) \
                   T( "#include \"interface.h\"\n\n" )

#define HEADER_START T( "#include <windows.h>\n" ) \
                     T( "#include \"twc.h\"\n\n" )

/* BOM */
static wchar_t BOM = 0xFEFF;

/* File descriptors of code and header files */
static FILE *fd_code, *fd_header;

/**
 * Make style and exnended style strings from properties.
 */
static int MakeStyleStrings( TWC_OBJECT *obj,      /* object */
                             TCHAR **style_str,   /* (out) pointer to style string */
                             TCHAR **exstyle_str) /* (out) pointer to extended style string */
{
    TCHAR *style_s, *exstyle_s;
    TCHAR *p_s, *p_ex;
    PROP_CODEGEN_ACTION *act;
    VALUE *val;
    PROPERTY_INFO *propinfo;
    UINT prop_id, prop_count, prop_flags;
    PROP_CODEGEN_CUSTOM_ACT_FUNC custom_act;

    style_s = malloc( 2048 * sizeof(TCHAR));
    exstyle_s = malloc( 2048 * sizeof(TCHAR));

    p_s = style_s;
    p_ex = exstyle_s;
    *p_s = T('\0');
    *p_ex = T('\0');

    /* Common actions */
    custom_act = GetControlCodegenCustomAct( obj->id);
    if ( custom_act != NULL ) {
        custom_act( obj, PROPERTIES_ALL, &p_s, &p_ex);
    }

    /* Property-specific actions */
    prop_count = GetControlPropertiesCount( obj->id);
    for ( prop_id = COMMON_PROPERTIES_BEGIN; prop_id < prop_count; prop_id++ ) {
        prop_flags = GetObjectPropertyFlags( obj, prop_id);
        propinfo = GetPropertyInfo( obj->id, prop_id);
        val = GetObjectPropertyVal( obj, prop_id);

        act = &(GetPropertyInfo( obj->id, prop_id)->on_codegen_act);

        if ( act->style_to_set != NULL && val->i == 1 ) {
            p_s = _mytcscpy( p_s, act->style_to_set);
            p_s = _mytcscpy( p_s, _T(" | "));
        }

        if ( act->style_list_to_set != NULL && act->style_list_to_set[val->i] != NULL ) {
            p_s = _mytcscpy( p_s, act->style_list_to_set[val->i]);
            p_s = _mytcscpy( p_s, _T(" | "));
        }

        if ( act->exstyle_to_set != NULL && val->i == 1 ) {
            p_ex = _mytcscpy( p_ex, act->exstyle_to_set);
            p_ex = _mytcscpy( p_ex, _T(" | "));
        }

        if ( act->exstyle_list_to_set != NULL && act->exstyle_list_to_set[val->i] != NULL ) {
            p_ex = _mytcscpy( p_ex, act->exstyle_list_to_set[val->i]);
            p_ex = _mytcscpy( p_ex, _T(" | "));
        }

        if ( act->custom_act_func != NULL ){
            act->custom_act_func( obj, prop_id, &p_s, &p_ex);
        }
    }

    /* Free (if empty) or NULL-terminate style end extended style strings */
    if ( *style_s == '\0' ) {
        free( style_s);
        style_s = NULL;
    } else {
        *(p_s - 3) = '\0';
    }

    if ( *exstyle_s == '\0' ) {
        free( exstyle_s);
        exstyle_s = NULL;
    } else {
        *(p_ex - 3) = '\0';
    }

    *style_str = style_s;
    *exstyle_str = exstyle_s;

    return 1;
}

/**
 * Get object full name length.
 */
static int GetObjectNameLen( TWC_OBJECT *obj) /* object */
{
    int len = 0;

    if ( !IsObjectRoot( obj->parent) ) {
        len += GetObjectNameLen( obj->parent) + 1;
    }
    len += _tcslen( obj->name);

    return len;
}

/**
 * Print object full name to buffer.
 */
static TCHAR *PrintObjectName( TCHAR *buf,     /* buffer */
                               TWC_OBJECT *obj) /* object */
{
    TCHAR *p;

    p = buf;
    if ( !IsObjectRoot( obj->parent) ) {
        p = PrintObjectName( p, obj->parent);
        *p++ = '_';
    }
    p = _mytcscpy( p, obj->name);
    return p;
}

/**
 * Get object classname to write (const for controls and variable for windows).
 */
static const TCHAR *GetObjectClassnameToWrite( TWC_OBJECT *obj) /* object */
{
    if ( obj->id == CTRL_ID_WINDOW ) {
        return GetObjectPropertyVal( obj, WINDOW_CLASSNAME)->s;
    } else {
        return GetControlClassnameTWC( obj->id);
    }
}

/**
 * Generate code for object.
 */
static int GenerateObjectCode( TWC_OBJECT *obj) /* object */
{
    TCHAR *buf;
    int bufsize, objnamelen;
    TCHAR *p, *p1;
    TCHAR *styles_str, *exstyles_str;

    if ( obj->name == NULL ) {
        MessageBox( hMainWnd, T("One of objects doesn't have name!"), T("Error"), 0);
        return 0;
    }

    /* Make style and extended style strings */
    MakeStyleStrings( obj, &styles_str, &exstyles_str);

    /* Calculate buffer size */
    bufsize = 0;
    objnamelen = GetObjectNameLen( obj);
    bufsize += objnamelen;
    if ( obj->child_list.first != NULL ) {
        bufsize += objnamelen * 2;
        bufsize += 14 /*2x "_childs"*/ + 11 /* "TWC_OBJECT " */ + 3 /* "*[]" */ + 8 /* " = {\n};\n" */;
        bufsize += 6; // "\tNULL\n"
    } else {
        bufsize += 4; //"NULL"
    }
    bufsize += 11 /* "TWC_OBJECT " */ + 8 /* " = {\n};\n" */;
    bufsize += ( styles_str != NULL) ? _tcslen( styles_str) + 8 : 1;
    bufsize += ( exstyles_str != NULL) ? _tcslen( exstyles_str) + 8 : 1;
    bufsize += _tcslen( GetObjectClassnameToWrite( obj)) + (( obj->title ) ? _tcslen( obj->title) : 0);
    bufsize += 7 * 3; // 7x "\t,\n"
    bufsize += 4; // 2x ", "
    bufsize += 4 * 15; //ints
    bufsize += 50; // '\0' + unexpected stuff

    /* Generate code for child objects */
    OBJ_LIST_ITERATE_BEGIN( &obj->child_list);
        if ( GenerateObjectCode( node->elem) == 0 ) {
            return 0;
        }
        bufsize += objnamelen + 5;
        bufsize += GetObjectNameLen( node->elem);
    OBJ_LIST_ITERATE_END();

    /* Allocate buffer */
    buf = malloc( bufsize * sizeof(TCHAR));
    p = buf;

    /* Generate child array */
    if ( obj->child_list.first != NULL ) {
        p = _mytcscpy(p, T("TWC_SOBJECT *"));
        p = PrintObjectName(p, obj);
        p = _mytcscpy(p, T("_childs"));
        p = _mytcscpy(p, T("[] = {\n"));

        OBJ_LIST_ITERATE_BEGIN( &obj->child_list);
            *p++ = '\t'; *p++ = '&';
            p = PrintObjectName( p, node->elem);
            *p++ = ','; *p++ = '\n';
        OBJ_LIST_ITERATE_END();

        *p++ = '\t';
        p = _mytcscpy( p, T("NULL\n};\n\n"));
    }

    /* Generate code for object */
    p1 = p;
    p = _mytcscpy( p, T("TWC_SOBJECT "));
    p = PrintObjectName( p, obj);

    /* Write object name to header file */
    fwrite( T("extern "), 7 * sizeof(TCHAR), 1, fd_header);
    fwrite( p1, (p - p1) * sizeof(TCHAR), 1, fd_header);
    fwrite( T(";\n"), 2 * sizeof(TCHAR), 1, fd_header);

    /* Write classname */
    p = _mytcscpy( p, T(" = {\n\t"));
    if ( obj->id == CTRL_ID_WINDOW ) {
        p = _mytcscpy( p, T("TEXT(\""));
        p = _mytcscpy( p, GetObjectClassnameToWrite( obj));
        p = _mytcscpy( p, T("\")"));
    } else {
        p = _mytcscpy( p, GetObjectClassnameToWrite( obj));
    }
    p = _mytcscpy( p, T(",\n\t"));

    /* Write position and size */
    if (obj->x == CW_USEDEFAULT) {
        p = _mytcscpy( p, T("CW_USEDEFAULT, "));
    } else {
        _itot( obj->x, p, 10); p += _tcslen( p); *p++ = ','; *p++ = ' ';
    }
    _itot( obj->y, p, 10); p += _tcslen( p); *p++ = ','; *p++ = '\n'; *p++ = '\t';
    _itot( obj->width, p, 10); p += _tcslen( p); *p++ = ','; *p++ = ' ';
    _itot( obj->height, p, 10); p += _tcslen( p); *p++ = ','; *p++ = '\n'; *p++ = '\t';

    /* Write title */
    if ( obj->title ) {
        p = _mytcscpy( p, T("TEXT(\""));
        p = _mytcscpy (p, obj->title);
        p = _mytcscpy( p, T("\"),\n\t"));
    } else {
        p = _mytcscpy( p, T("NULL,\n\t"));
    }

    /* Write styles string */
    if ( styles_str ) {
        p = _mytcscpy( p, styles_str);
    } else {
        *p++ = T('0');
    }
    *p++ = ','; *p++ = '\n'; *p++ = '\t';

    /* Write exstyles string */
    if ( exstyles_str ) {
        p = _mytcscpy( p, exstyles_str);
    } else {
        *p++ = T('0');
    }
    *p++ = ','; *p++ = '\n'; *p++ = '\t';

    /* Write flags */
    if ( obj->flags != 0 ) {
        if ( obj->flags & OBJ_FLAG_CLIENTSIZE ) {
            p = _mytcscpy( p, T("TWC_OBJ_FLAG_CLIENTSIZE"));
        }
    } else {
        *p++ = T('0');
    }
    *p++ = ','; *p++ = '\n'; *p++ = '\t';

    /* Print childs array name */
    if ( obj->child_list.first != NULL ) {
        p = PrintObjectName( p, obj);
        p = _mytcscpy( p, T("_childs\n};\n\n"));
    } else {
        p = _mytcscpy( p, T("NULL\n};\n\n"));
    }

    /* Write buffer to file */
    fwrite( buf, (p - buf) * sizeof(TCHAR), 1, fd_code);

    free(buf);
    if ( styles_str ) free( styles_str);
    if ( exstyles_str ) free( exstyles_str);
    return 1;
}

/**
 * Generate code for project.
 */
int GenerateProjectCode( TWCD_PROJECT *project) /* project */
{
    SetCurrentDirToExeFolder();

    /* Create files */
    fd_code = _tfopen( T("code\\interface.c"), T("wb"));
    if ( !fd_code ) {
        return 0;
    }
    fd_header = _tfopen( T("code\\interface.h"), T("wb"));
    if ( !fd_header ) {
        fclose(fd_code);
        return 0;
    }

    /* Write BOM if UNICODE */
#ifdef UNICODE
    fwrite( &BOM, 2, 1, fd_code);
    fwrite( &BOM, 2, 1, fd_header);
#endif

    /* Write code and header files headers */
    fwrite( CODE_START, sizeof(CODE_START) - sizeof(TCHAR), 1, fd_code);
    fwrite( HEADER_START, sizeof(HEADER_START) - sizeof(TCHAR), 1, fd_header);

    /* Write objects code */
    OBJ_LIST_ITERATE_BEGIN( GetProjectChildList( project));
        if ( GenerateObjectCode( node->elem) == 0 ) break;
    OBJ_LIST_ITERATE_END();

    fclose( fd_code);
    fclose( fd_header);

    return 1;
}
