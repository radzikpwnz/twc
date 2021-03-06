#ifndef LIBTWC_CTRL_INFO_H
#define LIBTWC_CTRL_INFO_H

#include <windows.h>

//////////////////
// Control list //
//////////////////

enum CONTROL_ID_INTERNAL {
    CTRL_ID_UNDEFINED = 0,
    CTRL_ID_ROOT,
    CONTROL_FIRST_ID,
};

enum CONTROL_ID {
	CTRL_ID_WINDOW = CONTROL_FIRST_ID,
	CTRL_ID_BUTTON,
	CTRL_ID_CHECKBOX,
	CTRL_ID_RADIOBUTTON,
	CTRL_ID_EDIT,
	CTRL_ID_LISTBOX,
	CTRL_ID_COMBOBOX,
	CTRL_ID_STATIC,
	CTRL_ID_GROUPBOX,
	CTRL_ID_SLIDER,
	CTRL_ID_PROGRESSBAR,
	CTRL_ID_IPADDRESS,
	CTRL_ID_DATETIME,
	CTRL_ID_SCROLLBAR,
	CTRL_ID_UPDOWN,
	CTRL_ID_STATUSBAR,
	CTRL_ID_TOOLBAR,
    CONTROL_LAST_ID
};

#define CONTROL_COUNT (CONTROL_LAST_ID - CONTROL_FIRST_ID + 1)

//Property types
enum TYPE {
	T_INT,
	T_STR,
	T_LIST,
	T_BOOL,
	T_DOUBLE
};

typedef union tagVALUE{
	HANDLE h;
	TCHAR *s;
	int i;
} VALUE;


enum {
    PROPERTY_ON_APPLY_NONE = 0,
    PROPERTY_ON_APPLY_RECREATE = 1 << 0,
    PROPERTY_ON_APPLY_SETSTYLE = 1 << 1,
    PROPERTY_ON_APPLY_SETEXSTYLE = 1 << 2,
    PROPERTY_ON_APPLY_REDRAW = 1 << 3,
    PROPERTY_ON_APPLY_SETSTYLE_AND_REDRAW = PROPERTY_ON_APPLY_SETSTYLE | PROPERTY_ON_APPLY_REDRAW,
    PROPERTY_ON_APPLY_SETEXSTYLE_AND_REDRAW = PROPERTY_ON_APPLY_SETEXSTYLE | PROPERTY_ON_APPLY_REDRAW
};

#define PROPERTY_ON_APPLY_CUSTOM_SHIFT 16
#define PROPERTY_ON_APPLY_CUSTOM_MASK (((1 << 16) - 1) << PROPERTY_ON_APPLY_CUSTOM_SHIFT)

enum {
    PROPERTY_ON_APPLY_CUSTOM_NONE = 0,
    PROPERTY_ON_APPLY_CUSTOM_SETPOS,
    PROPERTY_ON_APPLY_CUSTOM_SETSIZE,
    PROPERTY_ON_APPLY_CUSTOM_SETTITLE,
    PROPERTY_ON_APPLY_CUSTOM_END
};


typedef void (*PROP_ON_SET_CUSTOM_ACT_FUNC)( TWC_OBJECT *, UINT);

//Action on set info
typedef struct tagPROP_ON_SET_ACTION {
    DWORD style_to_set;
    DWORD exstyle_to_set;
    DWORD *style_list_to_set;
    DWORD *exstyle_list_to_set;
    DWORD style_to_remove;
    DWORD exstyle_to_remove;
    PROP_ON_SET_CUSTOM_ACT_FUNC custom_act_func;
} PROP_ON_SET_ACTION;


typedef void (*PROP_CODEGEN_CUSTOM_ACT_FUNC)( TWC_OBJECT *, UINT, TCHAR **, TCHAR **);

//Action on codegen info
typedef struct tagPROP_CODEGEN_ACTION {
    TCHAR *style_to_set;
    TCHAR *exstyle_to_set;
    TCHAR **style_list_to_set;
    TCHAR **exstyle_list_to_set;
    PROP_CODEGEN_CUSTOM_ACT_FUNC custom_act_func;
} PROP_CODEGEN_ACTION;


//Property info
typedef struct tagPROPERTY_INFO {
	TCHAR *name;
	UINT type;
	VALUE default_caption;
	TCHAR *dname;
	UINT_PTR opt;
    PROP_ON_SET_ACTION on_set_act;
    PROP_CODEGEN_ACTION on_codegen_act;
    UINT on_apply_act;
} PROPERTY_INFO;


//Control info
typedef struct tagCONTROL_INFO {
    TCHAR *class_name;
    TCHAR *class_name_twc;
    UINT properties_count;
    PROPERTY_INFO *property_info;
    TCHAR *def_obj_name;
    PROP_CODEGEN_CUSTOM_ACT_FUNC codegen_custom_act;
} CONTROL_INFO;

//=====================================//


////////////////
// PROPERTIES //
////////////////

#define PROPERTIES_ALL ( (UINT)-1 )
#define PROPERTY_UNDEFINED ( (UINT)-2 )

////////////
// COMMON //
////////////

#define COMMON_PROPERTIES_BEGIN 0

enum COMMON_PROPERTIES {
	COMMON_NAME = COMMON_PROPERTIES_BEGIN,
    COMMON_UNIQUE_NAME,
	COMMON_X,
	COMMON_Y,
	COMMON_WIDTH,
	COMMON_HEIGHT,
	COMMON_TITLE,
	COMMON_BORDER,
	//COMMON_TEXTALIGN,
	COMMON_DISABLED,
	//COMMON_GROUP,
	COMMON_VISIBLE,
	COMMON_CLIENTEDGE,
	COMMON_CUSTOMSTYLE,
    COMMON_CUSTOMEXSTYLE,
	//COMMON_DLGFRAME,
	//COMMON_TRANSPARENT,
	//COMMON_CLIENTEDGE,
    COMMON_PROPERTIES_END,
};

#define COMMON_PROPERTIES_COUNT (COMMON_PROPERTIES_END - COMMON_PROPERTIES_BEGIN)


/////////////
// WINDOW  //
/////////////
enum WINDOW_PROPERTIES {
    WINDOW_PROPERTIES_BEGIN = COMMON_PROPERTIES_END,

    WINDOW_CLIENTSIZE = WINDOW_PROPERTIES_BEGIN,
    WINDOW_CLASSNAME,
    WINDOW_TOOLWINDOW,
    WINDOW_CODEFILE,

    WINDOW_PROPERTIES_END
};

#define WINDOW_PROPERTIES_COUNT (WINDOW_PROPERTIES_END - COMMON_PROPERTIES_BEGIN)


////////////////
// BUTTON //
////////////////
enum BUTTON_PROPERTIES {
    BUTTON_PROPERTIES_BEGIN = COMMON_PROPERTIES_END,

	BTN_OWNERDRAW = BUTTON_PROPERTIES_BEGIN,
	BTN_DEFAULT,
	BTN_BITMAP,
	BTN_FLAT,
	BTN_MULTILINE,
	BTN_NOTIFY,
	BTN_VERTALIGN,
	BTN_HORALIGN,

    BUTTON_PROPERTIES_END
};

#define BUTTON_PROPERTIES_COUNT (BUTTON_PROPERTIES_END - COMMON_PROPERTIES_BEGIN)


////////////////////
// CHECK BOX //
///////////////////
enum CHECKBOX_PROPERTIES {
    CHECKBOX_PROPERTIES_BEGIN = COMMON_PROPERTIES_END,

	CHK_OWNERDRAW = CHECKBOX_PROPERTIES_BEGIN,
	CHK_AUTO,
	CHK_3STATE,
	CHK_LEFTTEXT,
	CHK_BITMAP,
	CHK_FLAT,
	CHK_MULTILINE,
	CHK_PUSHLIKE,
	CHK_NOTIFY,
	CHK_VERTALIGN,
	CHK_HORALIGN,

    CHECKBOX_PROPERTIES_END
};

#define CHECKBOX_PROPERTIES_COUNT (CHECKBOX_PROPERTIES_END - COMMON_PROPERTIES_BEGIN)


/////////////////////////
// RADIO BUTTON //
////////////////////////
enum RADIOBUTTON_PROPERTIES {
    RADIOBUTTON_PROPERTIES_BEGIN = COMMON_PROPERTIES_END,

	RADIO_OWNERDRAW = RADIOBUTTON_PROPERTIES_BEGIN,
	RADIO_AUTO,
	RADIO_LEFTTEXT,
	RADIO_BITMAP,
	RADIO_FLAT,
	RADIO_MULTILINE,
	RADIO_PUSHLIKE,
	RADIO_NOTIFY,
	RADIO_VERTALIGN,
	RADIO_HORALIGN,

    RADIOBUTTON_PROPERTIES_END
};

#define RADIOBUTTON_PROPERTIES_COUNT (RADIOBUTTON_PROPERTIES_END - COMMON_PROPERTIES_BEGIN)


////////////
// EDIT //
///////////
enum EDIT_PROPERTIES {
    EDIT_PROPERTIES_BEGIN = COMMON_PROPERTIES_END,

	EDIT_AUTOHSCROLL = EDIT_PROPERTIES_BEGIN,
	EDIT_AUTOVSCROLL,
	EDIT_LOWERCASE,
	EDIT_MULTILINE,
	EDIT_NOHIDESEL,
	EDIT_NUMBER,
	EDIT_OEMCONVERT,
	EDIT_PASSWORD,
	EDIT_READONLY,
	EDIT_UPPERCASE,
	EDIT_VSCROLL,
	EDIT_HSCROLL,
	EDIT_ALIGN,

    EDIT_PROPERTIES_END
};

#define EDIT_PROPERTIES_COUNT (EDIT_PROPERTIES_END - COMMON_PROPERTIES_BEGIN)


/////////////////
// LIST BOX //
////////////////
enum LISTBOX_PROPERTIES {
    LISTBOX_PROPERTIES_BEGIN = COMMON_PROPERTIES_END,

	LISTBOX_DISABLENOSCROLL = LISTBOX_PROPERTIES_BEGIN,
	LISTBOX_HASSTRINGS,
	LISTBOX_MULTICOLUMN,
	//LISTBOX_NODATA,
	LISTBOX_NOINTEGRALHEIGHT,
	LISTBOX_NOREDRAW,
	LISTBOX_NOTIFY,
	LISTBOX_SORT,
	LISTBOX_USETABSTOPS,
	LISTBOX_WANTKEYINPUT,
	LISTBOX_VSCROLL,
	LISTBOX_HSCROLL,
	LISTBOX_SELECTION,
	//LISTBOX_OWNERDRAW

    LISTBOX_PROPERTIES_END
};

#define LISTBOX_PROPERTIES_COUNT (LISTBOX_PROPERTIES_END - COMMON_PROPERTIES_BEGIN)


///////////////
// COMBO BOX //
///////////////
enum COMBOBOX_PROPERTIES {
    COMBOBOX_PROPERTIES_BEGIN = COMMON_PROPERTIES_END,

	COMBOBOX_AUTOHSCROLL = COMBOBOX_PROPERTIES_BEGIN,
	COMBOBOX_DISABLENOSCROLL,
	COMBOBOX_HASSTRINGS,
	COMBOBOX_LOWERCASE,
	COMBOBOX_NOINTEGRALHEIGHT,
	COMBOBOX_OEMCONVERT,
	COMBOBOX_SORT,
	COMBOBOX_UPPERCASE,
	//COMBOBOX_OWNERDRAW
	COMBOBOX_TYPE,

    COMBOBOX_PROPERTIES_END
};

#define COMBOBOX_PROPERTIES_COUNT (COMBOBOX_PROPERTIES_END - COMMON_PROPERTIES_BEGIN)


////////////
// STATIC //
////////////
enum STATIC_PROPERTIES {
    STATIC_PROPERTIES_BEGIN = COMMON_PROPERTIES_END,

	STATIC_TYPE = STATIC_PROPERTIES_BEGIN,
	STATIC_CENTERIMAGE,
	STATIC_EDITCONTROL,
	STATIC_ENDELLIPSIS,
	STATIC_NOPREFIX,
	STATIC_NOTIFY,
	STATIC_PATHELLIPSIS,
	STATIC_REALSIZECONTROL,
	STATIC_REALSIZEIMAGE,
	STATIC_RIGHTJUST,
	STATIC_SUNKEN,
	STATIC_WORDELLIPSIS,

    STATIC_PROPERTIES_END
};

#define STATIC_PROPERTIES_COUNT (STATIC_PROPERTIES_END - COMMON_PROPERTIES_BEGIN)


//////////////
// GROUPBOX //
//////////////
enum GROUPBOX_PROPERTIES {
    GROUPBOX_PROPERTIES_BEGIN = COMMON_PROPERTIES_END,

	GROUPBOX_HORALIGN = GROUPBOX_PROPERTIES_BEGIN,

    GROUPBOX_PROPERTIES_END
};

#define GROUPBOX_PROPERTIES_COUNT (GROUPBOX_PROPERTIES_END - COMMON_PROPERTIES_BEGIN)


////////////
// SLIDER //
////////////
enum SLIDER_PROPERTIES {
    SLIDER_PROPERTIES_BEGIN = COMMON_PROPERTIES_END,

    SLIDER_AUTOTICKS = SLIDER_PROPERTIES_BEGIN,
    SLIDER_ENABLESELRANGE,
    SLIDER_NOTHUMB,
    SLIDER_TICKS,
    SLIDER_TOOLTIPS,
    SLIDER_VERT,

    SLIDER_PROPERTIES_END
};

#define SLIDER_PROPERTIES_COUNT (SLIDER_PROPERTIES_END - COMMON_PROPERTIES_BEGIN)


/////////////////
// PROGRESSBAR //
/////////////////
enum PROGRESSBAR_PROPERTIES {
    PROGRESSBAR_PROPERTIES_BEGIN = COMMON_PROPERTIES_END,

    PROGRESSBAR_MARQuEE = PROGRESSBAR_PROPERTIES_BEGIN,
    PROGRESSBAR_SMOOTH,
    PROGRESSBAR_SMOOTHREVERSE,
    PROGRESSBAR_VERTICAL,

    PROGRESSBAR_PROPERTIES_END
};

#define PROGRESSBAR_PROPERTIES_COUNT (PROGRESSBAR_PROPERTIES_END - COMMON_PROPERTIES_BEGIN)


////////////////
// IP ADDRESS //
////////////////
enum IPADDRESS_PROPERTIES {
    IPADDRESS_PROPERTIES_BEGIN = COMMON_PROPERTIES_END,

    IPADDRESS_PROPERTIES_END
};

#define IPADDRESS_PROPERTIES_COUNT (COMMON_PROPERTIES_END)


/////////////////
// DATETIME //
/////////////////
enum DATETIME_PROPERTIES {
    DATETIME_PROPERTIES_BEGIN = COMMON_PROPERTIES_END,

    DATETIME_APPCANPARSE = DATETIME_PROPERTIES_BEGIN,
    DATETIME_FORMAT,
    DATETIME_RIGHTALIGN,
    DATETIME_SHOWNONE,
    DATETIME_UPDOWN,

    DATETIME_PROPERTIES_END
};

#define DATETIME_PROPERTIES_COUNT (DATETIME_PROPERTIES_END - COMMON_PROPERTIES_BEGIN)


///////////////
// SCROLLBAR //
///////////////
enum SCROLLBAR_PROPERTIES {
    SCROLLBAR_PROPERTIES_BEGIN = COMMON_PROPERTIES_END,

    SCROLLBAR_PROPERTIES_END
};

#define SCROLLBAR_PROPERTIES_COUNT (COMMON_PROPERTIES_END)


////////////
// UPDOWN //
////////////
enum UPDOWN_PROPERTIES {
    UPDOWN_PROPERTIES_BEGIN = COMMON_PROPERTIES_END,

    UPDOWN_PROPERTIES_END
};

#define UPDOWN_PROPERTIES_COUNT (COMMON_PROPERTIES_END)


///////////////
// STATUSBAR //
///////////////
enum STATUSBAR_PROPERTIES {
    STATUSBAR_PROPERTIES_BEGIN = COMMON_PROPERTIES_END,

    STATUSBAR_PROPERTIES_END
};

#define STATUSBAR_PROPERTIES_COUNT (COMMON_PROPERTIES_END)


/////////////
// TOOLBAR //
/////////////
enum TOOLBAR_PROPERTIES {
    TOOLBAR_PROPERTIES_BEGIN = COMMON_PROPERTIES_END,

	TOOLBAR_LIST = TOOLBAR_PROPERTIES_BEGIN,
	//TOOLBAR_RIGHT

    TOOLBAR_PROPERTIES_END
};

#define TOOLBAR_PROPERTIES_COUNT (TOOLBAR_PROPERTIES_END - COMMON_PROPERTIES_BEGIN)



EXPORT CONTROL_INFO *twc_GetControlInfo( UINT ctrl_id);

EXPORT PROPERTY_INFO *twc_GetPropertyInfo( UINT ctrl_id, UINT prop_id);

EXPORT UINT twc_GetControlPropertiesCount( UINT ctrl_id);

EXPORT TCHAR *twc_GetControlClassname( UINT ctrl_id);

EXPORT TCHAR *twc_GetControlClassnameTWC( UINT ctrl_id);

EXPORT TCHAR *twc_GetControlDefaultObjectName( UINT ctrl_id);

EXPORT PROP_CODEGEN_CUSTOM_ACT_FUNC twc_GetControlCodegenCustomAct( UINT ctrl_id);

EXPORT UINT twc_GetPropertyIdByName( TCHAR *name, UINT ctrl_id);

#endif