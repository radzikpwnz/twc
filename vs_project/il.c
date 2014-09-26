#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#include "common.h"

#include "properties.h"
#include "mylist.h"

#include "il.h"


#define TWCIL_HEADER T("TWCIL 1.0\n\n")

#define COMMENT '#'
#define	WHITESPACE T("\n\r \t")

/**
 * Control string names
 * Order must match control id's order declared in "object.h"
 */
TCHAR *control_strings[] = {
    T("WINDOW"), /* not used */
	T("BUTTON"),
	T("CHECKBOX"),
	T("RADIOBUTTON"),
	T("EDIT"),
	T("LISTBOX"),
	T("COMBOBOX"),
	T("STATIC"),
	T("GROUPBOX"),
	T("SLIDER"),
	T("PROGRESSBAR"),
	T("IPADDRESS"),
	T("DATETIME"),
	T("SCROLLBAR"),
	T("UPDOWN"),
	T("STATUSBAR"),
	T("TOOLBAR")
};

/* Parse error codes */
typedef enum tagERR_CODE {
	ERR_SUCCESS, /* success */
	ERR_LEXICAL, /* lexical error */
	ERR_SYNTAX   /* syntax error */
} ERR_CODE;

/* Lexeme id's */
typedef enum tagLEX_ID {
	LEX_INVALID,  /* invalid lexeme */
	LEX_WINDOW,   /* window */
	LEX_CONTROL,  /* control */
	LEX_PROPID,   /* property ID*/
	LEX_INTVAL,   /* integer value */
	LEX_STRVAL,   /* string value */
	LEX_FLOATVAL, /* float value */
	LEX_END,      /* end of object */
	LEX_ENDSTREAM /* end of stream */
} LEX_ID;

/* Lexeme */
typedef struct tagLEX {
	LEX_ID id;
	VALUE val;
} LEX, *PLEX;


static TCHAR *filebuf;
static TCHAR *cur_ptr;
static UINT cur_obj_id;
static ERR_CODE parse_err;

/**
 * Get lexeme from input stream.
 */
static LEX_ID GetLex( LEX *lex) /* (out) lexeme */
{
	TCHAR *p, *p1;
	TCHAR **pp;
	int len;
	int i;
	TCHAR c;

	p = cur_ptr;
	lex->id = LEX_INVALID;

    /* Skip comments */
	while ( *p == COMMENT ) {
		p++;
		while ( *p++ != '\n' ) {
			if ( *p == '\0' ) break;
		}
		p += _tcsspn(p, WHITESPACE);
	}

	len = _tcscspn(p, WHITESPACE);
	if ( len == 0 )  {
		cur_ptr = p;
		lex->id = LEX_ENDSTREAM;
		goto ret;
	}

	if (_tcsncmp(p, T("WND"), 3) == 0) { //WINDOW
		p += len + _tcsspn(p + len, WHITESPACE);
		if (*p != '{')  goto ret;
		p += 1 + _tcsspn(p + 1, WHITESPACE);
		lex->id = LEX_WINDOW;
		cur_obj_id = CTRL_ID_WINDOW;
	} else if (_tcsncmp(p, T("CTRL"), 4) == 0) { //CONTROL
		p += len + _tcsspn(p + len, WHITESPACE);
		len = _tcscspn(p, WHITESPACE);
		if (len == 0)  goto ret;

		c = *(p + len);
		*(p + len) = '\0';
		for (pp = control_strings, i = 0; *pp != NULL; i++, pp++) {
			if (_tcscmp(p, *pp) == 0) {
				lex->val.i = i + CONTROL_FIRST_ID;
				break;
			}
		}
		if (*pp == NULL) goto ret;
		*(p + len) = c;

		p += len + _tcsspn(p + len, WHITESPACE);
		if (*p != '{') goto ret;
		p += 1 + _tcsspn(p + 1, WHITESPACE);
		lex->id = LEX_CONTROL;
		cur_obj_id = lex->val.i;
	} else if (*p == '}') { //End of item
		lex->id = LEX_END;
		p += 1 + _tcsspn(p + 1, WHITESPACE);
	} else { //Property name or value
		if (*p == '"') { //String value
			p++;
			p1 = p;
			p1 += _tcscspn(p1, T("\"\n\t\r"));
			if (*p1 != '\"') goto ret;
			len = p1 - p;
			lex->id = LEX_STRVAL;
			/*if (len == 0) {
				lex->val.s = NULL;
			} else {*/
            lex->val.s = malloc((len + 1) * sizeof(TCHAR));
			_tcsncpy(lex->val.s, p, len);
			*((TCHAR *)lex->val.s + len) = '\0';
			//}
			p1++;
			p = p1 + _tcsspn(p1, WHITESPACE);
		} else if (*p >= '0' && *p <= '9') { //Integer value
			lex->val.i = _ttoi(p);
			lex->id = LEX_INTVAL;
			p += len + _tcsspn(p + len, WHITESPACE);
		} else { //Property name
			i = 0;
			p1 = p;
			p1 += _tcscspn(p1, T("=\n\t \r"));
			if (*p1 != '=') goto ret;
			*p1 = '\0';

            i = GetPropertyIdByName( p, cur_obj_id);
            if ( i != PROPERTY_UNDEFINED ) {
                lex->id = LEX_PROPID;
				lex->val.i = i;
            }

			*p1 = '=';
			p1++;
			p = p1 + _tcsspn(p1, WHITESPACE);
		}
	}

	cur_ptr = p;

ret:
	return lex->id;
}

/**
 * Parse property value.
 */
static int ParsePropertyVal( RT_OBJECT *obj, /* object */
                             UINT prop_id)   /* property ID */
{
	LEX lex;
	PROPERTY_INFO *propinfo;

    /* Get lexeme and check type */
	GetLex( &lex);
	if ( lex.id != LEX_STRVAL && lex.id != LEX_INTVAL ) {
		parse_err = ( lex.id == LEX_INVALID ) ? ERR_LEXICAL : ERR_SYNTAX;
		return 0;
	}

    /* Compare property type and value type */
	propinfo = GetPropertyInfo( obj->ctrl_id, prop_id);
	if ( lex.id == LEX_STRVAL && propinfo->type != T_STR
         || lex.id == LEX_INTVAL && (propinfo->type != T_INT && propinfo->type != T_LIST && propinfo->type != T_BOOL) ) {
		parse_err = ERR_SYNTAX;
		return 0;
	}

    /* Set property */
    SetObjectProperty( obj, prop_id, &lex.val, TWC_TRUE, TWC_FALSE);

	return 1;
}

/**
 * Parse object.
 */
static int ParseObject( RT_OBJECT *parent) /* parent object */
{
    LEX lex;
    RT_OBJECT *obj;

    /* Create and prepare object */
    obj = NewObject( cur_obj_id);
    PrepareObject( obj);
    obj->parent = parent;

    while (1) {
        switch ( GetLex( &lex) ) {
            case LEX_PROPID:
                if ( ParsePropertyVal( obj, lex.val.i) == 0 ) goto err;
                break;
            case LEX_CONTROL:
            case LEX_WINDOW:
                if ( ParseObject( obj) == 0 ) goto err;
                break;
            case LEX_INVALID:
                parse_err = ERR_LEXICAL;
                return 0;
            case LEX_END:
                obj->lstnode_ptr = DListAdd( GetParentChildList( obj), (void *)-1, &obj);
                return 1;
            default:
                parse_err = ERR_SYNTAX;
                return 0;
        }
    }

err:
	free( obj);
	return 0;
}

/**
 * Parse buffer.
 */
static int Parse( TCHAR *buf) /* buffer */
{
	LEX lex;

	cur_ptr = buf;
	parse_err = ERR_SUCCESS;
	while ( *cur_ptr != '\0' ) {
        /* Buffer must contain window or be empty */
		if ( GetLex( &lex) != LEX_WINDOW ) {
			if ( lex.id != LEX_ENDSTREAM ) {
				parse_err = ( lex.id == LEX_INVALID ) ? ERR_LEXICAL : ERR_SYNTAX;
				break;
			}
		} else {
			if ( ParseObject(NULL) == 0 ) break;
		}
	}

	return ( parse_err == ERR_SUCCESS );
}

/**
 * Load file to buffer.
 */
static int LoadFile( TCHAR *path, /* file path */
                     void *buf)   /* buffer */
{
	FILE *fd;
	int filelen;
	int ret;

    /* Open file */
	fd = _tfopen( path, T("rb"));
	if ( !fd ) return 0;

    /* Calculate length */
	fseek(fd, 0, SEEK_END);
	filelen = ftell(fd);
	fseek(fd, 0, SEEK_SET);

    /* Alloc memory */
	filebuf = malloc( filelen + sizeof(TCHAR));
	if ( !filebuf ) goto err;

    /* Read file */
	ret = fread( filebuf, filelen, 1, fd);
	if ( ret != 1 ) goto err;

	*(TCHAR *)((char *)filebuf + filelen) = '\0';

	fclose( fd);
	return 1;

err:
	fclose( fd);
	if ( filebuf ) free( filebuf);
	return 0;
}

/**
 * Load project from file.
 */
int LoadProjectFromFile( TCHAR *path,           /* file path */
                         TWCD_PROJECT *project, /* (out) project */
                         int *err_pos)          /* (out) error position */
{
	int ret;
	TCHAR *p;
	int a = sizeof(RT_OBJECT);

    /* Read file */
	if ( err_pos ) *err_pos = 0;
	ret = LoadFile( path, filebuf);
	if ( !ret ) return 0;

    /* Compare signature */
	p = filebuf;
	if ( _tcsncmp( p, T("TWCIL "), 6) != 0 ) {
		goto err;
	}

    /* Get TWCIL version */
	p += 6;
	project->version.maj = _ttoi( p);
	while ( *p++ != '.' );
	project->version.min = _ttoi( p);
	while ( *p++ != '\n' );

    /* Parse file */
	cur_ptr = p + _tcsspn( p, WHITESPACE);
	if ( Parse(cur_ptr) == 0 ) {
		switch ( parse_err ) {
			case ERR_LEXICAL:
				if ( err_pos ) *err_pos = cur_ptr - filebuf;
				break;
			case ERR_SYNTAX:
				//if (err_pos) *err_pos = 0;
				break;
		}
		goto err;
	}

    /* Copy file path to project and free buffer */
	project->path = malloc( (_tcslen( path) + 1) * sizeof(TCHAR));
	_tcscpy( project->path, path);
	free( filebuf);
	return 1;

err:
	free(filebuf);
	return 0;
}

/**
 * Write object info to file.
 */
int WriteObjectInfo( FILE *fd,       /* file descriptor */
                     RT_OBJECT *obj) /* object */
{
	DLIST_NODE_PRT_OBJECT *cur_obj_node;
	static int depth;
	int i;
	TCHAR buf[256];
	TCHAR *cp;
	PROPERTY_INFO *propinfo;
	VALUE *val;
    UINT prop_id, prop_count, prop_flags;

	cp = buf;
	for (i = 0; i < depth; i++) {
		*cp++ = '\t';
	}
	if (obj->ctrl_id == CTRL_ID_WINDOW) {
		_tcscpy(cp, T("WND {\n"));
		cp += 6;
	} else {
		_tcscpy(cp,T("CTRL "));
		cp += 5;
		_tcscpy(cp, control_strings[obj->ctrl_id]);
		cp += _tcslen(control_strings[obj->ctrl_id]);
		_tcscpy(cp, T(" {\n"));
		cp += 3;
	}

	depth++;

	*cp = '\0';
	_ftprintf(fd, T("%s"), buf);

	cp = buf;

    prop_count = GetControlPropertiesCount( obj->ctrl_id);
    for ( prop_id = COMMON_PROPERTIES_BEGIN; prop_id < prop_count; prop_id++ ) {
        prop_flags = GetObjectPropertyFlags( obj, prop_id);
        if ( (prop_flags & PROPERTY_FLAG_SET) && !(prop_flags & PROPERTY_FLAG_DEFAULT) ) {
            propinfo = GetPropertyInfo( obj->ctrl_id, prop_id);
            val = GetObjectPropertyVal( obj, prop_id);

            for ( i = 0; i < depth; i++ ) {
			    *cp++ = '\t';
		    }

            _tcscpy( cp, propinfo->name);
		    cp += _tcslen( propinfo->name);
		    *cp++ = '=';
    		
		    switch ( propinfo->type ) {
			    case T_INT:
			    case T_LIST:
			    case T_BOOL:
				    _itot( val->i, cp, 10);
				    cp += _tcslen( cp);
				    *cp++ = '\n';
				    fwrite( buf, (cp - buf) * sizeof(TCHAR), 1, fd);
				    break;
			    case T_STR:
				    *cp++ = '"';
				    fwrite( buf, (cp - buf) * sizeof(TCHAR), 1, fd);
                    if ( val->s ) {
                        fwrite( val->s, _tcslen( val->s) * sizeof(TCHAR), 1, fd);
                    }
				    _fputtc('"', fd);
				    _fputtc('\n', fd);
				    break;
		    }
		    cp = buf;
        } 
    }

    OBJ_LIST_ITERATE_BEGIN( &obj->child_list);
		if ( WriteObjectInfo( fd, node->elem) == 0 ) {
			return 0;
		}
    OBJ_LIST_ITERATE_END();

	depth--;
	cp = buf;
	for (i = 0; i < depth; i++) {
		*cp++ = '\t';
	}
	*cp++ = '}';
	*cp++ = '\n';
	fwrite(buf, (cp - buf) * sizeof(TCHAR), 1, fd);
	return 1;
}

int SaveProjectToFile( TWCD_PROJECT *project, TCHAR *path)
{
	DLIST_NODE_PRT_OBJECT *cur_node;
	FILE *fd;
	TCHAR *buf;

	if (path == NULL) path = project->path;
	fd = _tfopen(path, T("wb, ccs=UNICODE"));
	if (!fd) return 0;

	(void *)cur_node =  project->obj_list.first;
	if (cur_node == NULL) goto ret;
	fwrite(TWCIL_HEADER, _tcslen(TWCIL_HEADER) * sizeof(TCHAR), 1, fd);
	while (cur_node != NULL) {
		if (WriteObjectInfo(fd, cur_node->elem) == 0) {
			buf = T("Error writing object");
			MessageBox(hMainWnd, buf, T("Error"), 0);
			fclose(fd);
			return 0;
		}
		cur_node = cur_node->next;
	}
	
ret:
	fclose(fd);
	return 1;
}
