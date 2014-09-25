#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#include "common.h"

#include "properties.h"
#include "mylist.h"

#include "il.h"

//Control string names
//Order must match control id's order declared in "object.h"
TCHAR *control_strings[] = {
    T("WINDOW"), //not used
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

//Parse error codes
typedef enum tagERR_CODE {
	ERR_SUCCESS,
	ERR_LEXICAL,
	ERR_SYNTAX
} ERR_CODE;

//Lexem id's
typedef enum tagLEX_ID {
	LEX_INVALID,
	LEX_WINDOW,
	LEX_CONTROL,
	LEX_PROPID,
	LEX_INTVAL,
	LEX_STRVAL,
	LEX_FLOATVAL,
	LEX_END,
	LEX_ENDSTREAM
} LEX_ID;

typedef struct tagLEX {
	LEX_ID id;
	VALUE val;
} LEX, *PLEX;

#define COMMENT '#'
#define	WHITESPACE T("\n\r \t")

//DLIST(LEX) lex_lst = {NULL, sizeof(LEX)};

TWCD_PROJECT *project;
TCHAR *filebuf;
TCHAR *cur_ptr;
UINT cur_obj_id;
ERR_CODE parse_err;

static LEX_ID GetLex(LEX *lex)
{
	TCHAR *p, *p1;
	TCHAR **pp;
	int len;
	int i;
	TCHAR c;

	p = cur_ptr;
	lex->id = LEX_INVALID;

	while (*p == COMMENT) { //skip comments
		p++;
		while (*p++ != '\n' ) {
			if (*p == '\0') break;
		}
		p += _tcsspn(p, WHITESPACE);
	}
	
	len = _tcscspn(p, WHITESPACE);
	if (len == 0)  {
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
	//if (lex->id != LEX_INVALID) {
		//DListAdd(&lex_lst, prev_elem, &lex);
	//}
	return lex->id;
}

static int ParsePropertyVal( RT_OBJECT *obj, UINT prop_id)
{
	LEX lex;
	PROPERTY_INFO *propinfo;

	GetLex(&lex);
	if (lex.id != LEX_STRVAL && lex.id != LEX_INTVAL) {
		parse_err = (lex.id == LEX_INVALID) ? ERR_LEXICAL : ERR_SYNTAX;
		goto err;
	}

	propinfo = GetPropertyInfo( obj->ctrl_id, prop_id);
	if ( lex.id == LEX_STRVAL && propinfo->type != T_STR
         || lex.id == LEX_INTVAL && (propinfo->type != T_INT && propinfo->type != T_LIST && propinfo->type != T_BOOL)) {
		parse_err = ERR_SYNTAX;
		goto err;
	}
	
    SetObjectProperty( obj, prop_id, &lex.val, TWC_TRUE, TWC_FALSE);

	return 1;
err:
	return 0;
}

static int ParseObject(RT_OBJECT *parent)
{
    LEX lex;
    RT_OBJECT *obj;

    obj = NewObject( cur_obj_id);
    PrepareObject( obj);
    obj->parent = parent;

    while (1) {
        switch (GetLex(&lex)) {
            case LEX_PROPID:
                if (ParsePropertyVal( obj, lex.val.i) == 0) goto err;
                break;
            case LEX_CONTROL:
            case LEX_WINDOW:
                if (ParseObject(obj) == 0) goto err;
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
	free(obj);
	return 0;
}

static int Parse(TCHAR *str)
{
	LEX lex;

	cur_ptr = str;
	parse_err = ERR_SUCCESS;
	while (*cur_ptr != '\0') {
		if (GetLex(&lex) != LEX_WINDOW) {
			if (lex.id != LEX_ENDSTREAM) {
				parse_err = (lex.id == LEX_INVALID) ? ERR_LEXICAL : ERR_SYNTAX;
				break;
			}
		} else {
			if (ParseObject(NULL) == 0) break;
		}
	}

	return (parse_err == ERR_SUCCESS);
}

static int LoadFile(TCHAR *path, void *buf)
{
	FILE *fd;
	int filelen;
	int ret;

	fd = _tfopen(path, T("rb"));
	if (!fd) return 0;

	fseek(fd, 0, SEEK_END);
	filelen = ftell(fd);
	fseek(fd, 0, SEEK_SET);

	filebuf = malloc(filelen + sizeof(TCHAR));
	if (!filebuf) goto err;

	ret = fread(filebuf, filelen, 1, fd);
	if (ret != 1) goto err;

	*(TCHAR *)((char *)filebuf + filelen) = '\0';

	fclose(fd);
	return 1;
err:
	fclose(fd);
	if (filebuf) free(filebuf);
	return 0;
}

int LoadProjectFromFile(TCHAR *path, TWCD_PROJECT *_project, int *err_pos)
{
	int ret;
	TCHAR *p;
	int a = sizeof(RT_OBJECT);

	project = _project;
	if (err_pos) *err_pos = 0;
	ret = LoadFile(path, filebuf);
	if (!ret) return 0;

	p = filebuf;
	if (_tcsncmp(p, T("TWCIL "), 6) != 0) {
		goto err;
	}

	p += 6;
	project->version.maj = _ttoi(p);
	while (*p++ != '.');
	project->version.min = _ttoi(p);
	while (*p++ != '\n');

	cur_ptr = p + _tcsspn(p, WHITESPACE);
	if (Parse(cur_ptr) == 0) {
		switch (parse_err) {
			case ERR_LEXICAL:
				if (err_pos) *err_pos = cur_ptr - filebuf;
				break;
			case ERR_SYNTAX:
				//if (err_pos) *err_pos = 0;
				break;
		}
		goto err;
	}

	project->path = malloc((_tcslen(path) + 1) * sizeof(TCHAR));
	_tcscpy(project->path, path);
	free(filebuf);
	return 1;

err:
	free(filebuf);
	return 0;
}

int WriteObjectInfo(FILE *fd, RT_OBJECT *obj)
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
	
	(void *)cur_obj_node =  obj->child_list.first;
	while (cur_obj_node != NULL) {
		if (WriteObjectInfo(fd, cur_obj_node->elem) == 0) {
			return 0;
		}
		cur_obj_node = cur_obj_node->next;
	}

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

int SaveProjectToFile(TWCD_PROJECT *project, TCHAR *path)
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