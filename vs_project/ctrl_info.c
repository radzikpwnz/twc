#include "twc_design.h"

#include "ctrl_info.h"


#define DECLARE_PROPERTY( name, type, default, dname, opt, on_set_act, on_codegen_act, on_apply_act) \
        {T(name), type, (HANDLE)default, T(dname), opt, on_set_act, on_codegen_act, on_apply_act},
#define DECLARE_PROPERTY_END \
        {NULL, 0, 0, NULL, 0, {0}, {0}}

#define DECLARE_PROPERTY_ON_SET_ACTION( set_style, set_exstyle, set_style_list, set_exstyle_list, rem_style, rem_exstyle, cust_act) \
        {(set_style), (set_exstyle), (set_style_list), (set_exstyle_list), (rem_style), (rem_exstyle), (cust_act)}

#define ON_SET_ACTION_SET_STYLE( set_style) \
        DECLARE_PROPERTY_ON_SET_ACTION( set_style, 0,           NULL, NULL, 0,         0,           NULL    )

#define ON_SET_ACTION_SET_EXSTYLE( set_exstyle) \
        DECLARE_PROPERTY_ON_SET_ACTION( 0,         set_exstyle, NULL, NULL, 0,         0,           NULL    )

#define ON_SET_ACTION_REM_STYLE_AND_SET_STYLELIST( rem_style, list) \
        DECLARE_PROPERTY_ON_SET_ACTION( 0,         0,           list, NULL, rem_style, 0,           NULL    )

#define ON_SET_ACTION_REM_EXSTYLE_AND_SET_EXSTYLELIST( rem_exstyle, list) \
        DECLARE_PROPERTY_ON_SET_ACTION( 0,         0,           NULL, list, 0,         rem_exstyle, NULL    )

#define ON_SET_ACTION_CUSTOM( cust_act) \
        DECLARE_PROPERTY_ON_SET_ACTION( 0,         0,           NULL, NULL, 0,         0,           cust_act)

#define ON_SET_ACTION_NONE \
        DECLARE_PROPERTY_ON_SET_ACTION( 0,         0,           NULL, NULL, 0,         0,           NULL    )


#define DECLARE_PROPERTY_CODEGEN_ACTION( set_style, set_exstyle, set_style_list, set_exstyle_list, cust_act) \
        {(set_style), (set_exstyle), (set_style_list), (set_exstyle_list), (cust_act)}

#define CODEGEN_ACTION_SET_STYLE( set_style) \
        DECLARE_PROPERTY_CODEGEN_ACTION( set_style, NULL,           NULL, NULL, NULL    )

#define CODEGEN_ACTION_SET_EXSTYLE( set_exstyle) \
        DECLARE_PROPERTY_CODEGEN_ACTION( NULL,      set_exstyle,    NULL, NULL, NULL    )

#define CODEGEN_ACTION_SET_STYLELIST( list) \
        DECLARE_PROPERTY_CODEGEN_ACTION( NULL,      NULL,           list, NULL, NULL    )

#define CODEGEN_ACTION_SET_EXSTYLELIST( list) \
        DECLARE_PROPERTY_CODEGEN_ACTION( NULL,      NULL,           NULL, list, NULL    )

#define CODEGEN_ACTION_CUSTOM( cust_act) \
        DECLARE_PROPERTY_CODEGEN_ACTION( NULL,      NULL,           NULL, NULL, cust_act)

#define CODEGEN_ACTION_NONE \
        DECLARE_PROPERTY_CODEGEN_ACTION( NULL,      NULL,           NULL, NULL, NULL)

#define ON_APPLY_ACTION( act) ( PROPERTY_ON_APPLY_##act )
#define ON_APPLY_ACTION_CUSTOM( act) ( PROPERTY_ON_APPLY_CUSTOM_##act << PROPERTY_ON_APPLY_CUSTOM_SHIFT )
#define ON_APPLY_ACTION_NONE ( PROPERTY_ON_APPLY_NONE )


/**
 * !!! All property info arrays must be fllled in order of propety id's declaration !!!
 */

static void PropOnSetCustomAction_Common( RT_OBJECT *obj, UINT prop_id)
{
    switch ( prop_id ) {
        case COMMON_NAME:
            obj->name = GetObjectPropertyVal( obj, prop_id)->s;
            break;
        case COMMON_X:
            obj->x = GetObjectPropertyVal( obj, prop_id)->i;
            break;
        case COMMON_Y:
            obj->y = GetObjectPropertyVal( obj, prop_id)->i;
            break;
        case COMMON_WIDTH:
            obj->width = GetObjectPropertyVal( obj, prop_id)->i;
            break;
        case COMMON_HEIGHT:
            obj->height = GetObjectPropertyVal( obj, prop_id)->i;
            break;
        case COMMON_TITLE:
            obj->title = GetObjectPropertyVal( obj, prop_id)->s;
            break;
    }

    return;
}

static void PropCodegenCustomAction_Common( RT_OBJECT *obj, UINT prop_id, TCHAR **style, TCHAR **exstyle)
{
    VALUE *val;

    switch ( prop_id ) {
        case COMMON_CUSTOMSTYLE:
	        val = GetObjectPropertyVal( obj, COMMON_CUSTOMSTYLE);
            if ( val->s[0] != T('\0') ) {
                *style = _mytcscpy( *style, val->s);
                *style = _mytcscpy( *style, _T(" | "));
            }
            break;
        case COMMON_CUSTOMEXSTYLE:
            val = GetObjectPropertyVal( obj, COMMON_CUSTOMEXSTYLE);
            if ( val->s[0] != T('\0') ) {
                *exstyle = _mytcscpy( *exstyle, val->s);
                *exstyle = _mytcscpy( *exstyle, _T(" | "));
            }
            break;
    }
    return;
}

PROPERTY_INFO common_propinfo[] = {
    DECLARE_PROPERTY( "name",             T_STR,   T("<unnamed>"),    "Name",                0, \
                      ON_SET_ACTION_CUSTOM( PropOnSetCustomAction_Common), \
                      CODEGEN_ACTION_CUSTOM( NULL), \
                      ON_APPLY_ACTION_NONE )
    DECLARE_PROPERTY( "uniquename",       T_BOOL,  0,    "Unique name",                      0, \
                      ON_SET_ACTION_NONE, \
                      CODEGEN_ACTION_NONE, \
                      ON_APPLY_ACTION_NONE )
    DECLARE_PROPERTY( "x",                T_INT,   0,                 "X",                   0, \
                      ON_SET_ACTION_CUSTOM( PropOnSetCustomAction_Common), \
                      CODEGEN_ACTION_CUSTOM( NULL), \
                      ON_APPLY_ACTION_CUSTOM( SETPOS) )
    DECLARE_PROPERTY( "y",                T_INT,   0,                 "Y",                   0, \
                      ON_SET_ACTION_CUSTOM( PropOnSetCustomAction_Common), \
                      CODEGEN_ACTION_CUSTOM( NULL), \
                      ON_APPLY_ACTION_CUSTOM( SETPOS) )
    DECLARE_PROPERTY( "width",            T_INT,   0,                 "Width",               0, \
                      ON_SET_ACTION_CUSTOM( PropOnSetCustomAction_Common), \
                      CODEGEN_ACTION_CUSTOM( NULL), \
                      ON_APPLY_ACTION_CUSTOM( SETSIZE) )
    DECLARE_PROPERTY( "height",           T_INT,   0,                 "Height",              0, \
                      ON_SET_ACTION_CUSTOM( PropOnSetCustomAction_Common), \
                      CODEGEN_ACTION_CUSTOM( NULL), \
                      ON_APPLY_ACTION_CUSTOM( SETSIZE) )
    DECLARE_PROPERTY( "title",            T_STR,   T(""),             "Caption",             0, \
                      ON_SET_ACTION_CUSTOM( PropOnSetCustomAction_Common), \
                      CODEGEN_ACTION_CUSTOM( NULL), \
                      ON_APPLY_ACTION_CUSTOM( SETTITLE) | ON_APPLY_ACTION( REDRAW) )
    DECLARE_PROPERTY( "border",           T_BOOL,  0,                 "Border",              0, \
                      ON_SET_ACTION_SET_STYLE( WS_BORDER), \
                      CODEGEN_ACTION_SET_STYLE( T("WS_BORDER")), \
                      ON_APPLY_ACTION( RECREATE) )
    DECLARE_PROPERTY( "disabled",         T_BOOL,  0,                 "Disabled",            0, \
                      ON_SET_ACTION_SET_STYLE( WS_DISABLED), \
                      CODEGEN_ACTION_SET_STYLE( T("WS_DISABLED")), \
                      ON_APPLY_ACTION( NONE) )
    DECLARE_PROPERTY( "visible",          T_BOOL,  1,                 "Visible",             0, \
                      ON_SET_ACTION_SET_STYLE( WS_VISIBLE), \
                      CODEGEN_ACTION_SET_STYLE( T("WS_VISIBLE")), \
                      ON_APPLY_ACTION( NONE) )
    DECLARE_PROPERTY( "clientedge",       T_BOOL,  0,                 "Client edge",         0, \
                      ON_SET_ACTION_SET_EXSTYLE( WS_EX_CLIENTEDGE), \
                      CODEGEN_ACTION_SET_EXSTYLE( T("WS_EX_CLIENTEDGE")), \
                      ON_APPLY_ACTION( SETEXSTYLE) )
    DECLARE_PROPERTY( "customstyle",      T_STR,   T(""),              "Custom style",        0, \
                      ON_SET_ACTION_NONE, \
                      CODEGEN_ACTION_CUSTOM( PropCodegenCustomAction_Common), \
                      ON_APPLY_ACTION( NONE) )
    DECLARE_PROPERTY( "customexstyle",    T_STR,   T(""),              "Custom exstyle",      0, \
                      ON_SET_ACTION_NONE, \
                      CODEGEN_ACTION_CUSTOM( PropCodegenCustomAction_Common), \
                      ON_APPLY_ACTION( NONE) )
    DECLARE_PROPERTY_END
};

void PropOnSetCustomAction_Window( RT_OBJECT *obj, UINT prop_id)
{
    VALUE *val;
    RECT rect;

    switch ( prop_id ) {
        case WINDOW_CLIENTSIZE:
            val = GetObjectPropertyVal( obj, prop_id);
            if ( val->i == 0 ) {
                if ( obj->hwnd != NULL ) {
                    GetWindowRect( obj->hwnd, &rect);
                    SetObjectPropertyInt( obj, COMMON_WIDTH, rect.right - rect.left, TWC_TRUE, TWC_FALSE);
                    SetObjectPropertyInt( obj, COMMON_HEIGHT, rect.bottom - rect.top, TWC_TRUE, TWC_FALSE);
                }
                obj->flags &= ~OBJ_FLAG_CLIENTSIZE;
            } else {
                if ( obj->hwnd != NULL ) {
                    GetClientRect( obj->hwnd, &rect);
                    SetObjectPropertyInt( obj, COMMON_WIDTH, rect.right - rect.left, TWC_TRUE, TWC_FALSE);
                    SetObjectPropertyInt( obj, COMMON_HEIGHT, rect.bottom - rect.top, TWC_TRUE, TWC_FALSE);
                }
                obj->flags |= OBJ_FLAG_CLIENTSIZE;
            }
            break;
    }
    return;
}

PROPERTY_INFO window_propinfo[] = {
    DECLARE_PROPERTY( "clientsize",       T_BOOL,   1,                "Use client size",     0, \
                      ON_SET_ACTION_CUSTOM( PropOnSetCustomAction_Window), \
                      CODEGEN_ACTION_NONE, \
                      ON_APPLY_ACTION( NONE) )
    DECLARE_PROPERTY( "class",            T_STR,   T("DefaultWnd"),   "Class name",          0, \
                      ON_SET_ACTION_NONE, \
                      CODEGEN_ACTION_NONE, \
                      ON_APPLY_ACTION( NONE) )
	DECLARE_PROPERTY( "toolwindow",       T_BOOL,  0,                 "Tool window",         0, \
                      ON_SET_ACTION_SET_EXSTYLE( WS_EX_TOOLWINDOW), \
                      CODEGEN_ACTION_SET_EXSTYLE( T("WS_EX_TOOLWINDOW")), \
                      ON_APPLY_ACTION( SETEXSTYLE) )
	DECLARE_PROPERTY( "codefile",         T_STR,   T(""),             "Code file",           0, \
                      ON_SET_ACTION_CUSTOM( PropOnSetCustomAction_Window), \
                      CODEGEN_ACTION_NONE, \
                      ON_APPLY_ACTION( NONE) )
	DECLARE_PROPERTY_END
};


static void PropOnSetCustomAction_Button( RT_OBJECT *obj, UINT prop_id)
{
    VALUE *val;

    obj->style &= ~BS_TYPEMASK;

    switch ( obj->ctrl_id ) {
        case CTRL_ID_BUTTON:
            break;
        case CTRL_ID_CHECKBOX:
			val = GetObjectPropertyVal( obj, CHK_AUTO);
            if (val->i == 1) {
				val = GetObjectPropertyVal( obj, CHK_3STATE);
                obj->style |= ( val->i == 1 ) ? BS_AUTO3STATE : BS_AUTOCHECKBOX;
			} else {
				val = GetObjectPropertyVal( obj, CHK_3STATE);
                obj->style |= ( val->i == 1) ? BS_3STATE : BS_CHECKBOX;
			}
            break;
        case CTRL_ID_RADIOBUTTON:
            val = GetObjectPropertyVal( obj, RADIO_AUTO);
			obj->style |= ( val->i == 1 ) ? BS_AUTORADIOBUTTON : BS_RADIOBUTTON;
            break;
        case CTRL_ID_GROUPBOX:
            break;
    }

    return;
}

static void PropCodegenCustomAction_Button( RT_OBJECT *obj, UINT prop_id, TCHAR **style, TCHAR **exstyle)
{
    VALUE *val;

    TWC_CHECKIT( prop_id == PROPERTIES_ALL );

    switch ( obj->ctrl_id ) {
        case CTRL_ID_BUTTON:
            break;
        case CTRL_ID_CHECKBOX:
			val = GetObjectPropertyVal( obj, CHK_AUTO);
            if (val->i == 1) {
				val = GetObjectPropertyVal( obj, CHK_3STATE);
                *style = _mytcscpy( *style, ( val->i == 1 ) ? T("BS_AUTO3STATE") : T("BS_AUTOCHECKBOX"));
			} else {
				val = GetObjectPropertyVal( obj, CHK_3STATE);
                *style = _mytcscpy( *style, ( val->i == 1 ) ? T("BS_3STATE") : T("BS_CHECKBOX"));
			}
            *style = _mytcscpy( *style, _T(" | "));
            break;
        case CTRL_ID_RADIOBUTTON:
            val = GetObjectPropertyVal( obj, RADIO_AUTO);
            *style = _mytcscpy( *style, ( val->i == 1 ) ? T("BS_AUTORADIOBUTTON") : T("BS_RADIOBUTTON"));
            *style = _mytcscpy( *style, _T(" | "));
            break;
        case CTRL_ID_GROUPBOX:
            break;
    }

    return;
}

TCHAR *button_vertalign_strings[] = {T("Top"), T("Center"), T("Bottom"), NULL};
DWORD button_vertalign_styles[] = {BS_TOP, BS_VCENTER, BS_BOTTOM};
TCHAR *button_vertalign_styles_str[] = {T("BS_TOP"), T("BS_VCENTER"), T("BS_BOTTOM")};

TCHAR *button_horalign_strings[] = {T("Left"), T("Center"), T("Right"), NULL};
DWORD button_horalign_styles[] = {BS_LEFT, BS_CENTER, BS_RIGHT};
TCHAR *button_horalign_styles_str[] = {T("BS_LEFT"), T("BS_CENTER"), T("BS_RIGHT")};

PROPERTY_INFO button_propinfo[] = {
	DECLARE_PROPERTY( "ownerdraw",        T_BOOL,  0,                 "Owner draw",          0, \
                      ON_SET_ACTION_NONE, \
                      CODEGEN_ACTION_SET_STYLE( T("BS_OWNERDRAW")), \
                      ON_APPLY_ACTION( NONE) )
	DECLARE_PROPERTY( "default",          T_BOOL,  0,                 "Default button",      0, \
                      ON_SET_ACTION_SET_STYLE( BS_DEFPUSHBUTTON), \
                      CODEGEN_ACTION_SET_STYLE( T("BS_DEFPUSHBUTTON")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "bitmap",           T_BOOL,  0,                 "Bitmap",              0, \
                      ON_SET_ACTION_SET_STYLE( BS_ICON), \
                      CODEGEN_ACTION_SET_STYLE( T("BS_ICON")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "flat",             T_BOOL,  0,                 "Flat",                0, \
                      ON_SET_ACTION_SET_STYLE( BS_FLAT), \
                      CODEGEN_ACTION_SET_STYLE( T("BS_FLAT")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "multiline",        T_BOOL,  0,                 "Multi-line",          0, \
                      ON_SET_ACTION_SET_STYLE( BS_MULTILINE), \
                      CODEGEN_ACTION_SET_STYLE( T("BS_MULTILINE")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "notify",           T_BOOL,  0,                 "Notify",              0, \
                      ON_SET_ACTION_NONE, \
                      CODEGEN_ACTION_SET_STYLE( T("BS_NOTIFY")), \
                      ON_APPLY_ACTION( NONE) )
	DECLARE_PROPERTY( "vertalign",        T_LIST,  1,                 "Vertical align",      (UINT_PTR)button_vertalign_strings, \
                      ON_SET_ACTION_REM_STYLE_AND_SET_STYLELIST( BS_VCENTER, button_vertalign_styles), \
                      CODEGEN_ACTION_SET_STYLELIST( button_vertalign_styles_str), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "horalign",         T_LIST,  1,                 "Horizontal align",    (UINT_PTR)button_horalign_strings, \
                      ON_SET_ACTION_REM_STYLE_AND_SET_STYLELIST( BS_CENTER, button_horalign_styles), \
                      CODEGEN_ACTION_SET_STYLELIST( button_horalign_styles_str), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY_END
};


#define PropOnSetCustomAction_Checkbox PropOnSetCustomAction_Button

PROPERTY_INFO checkbox_propinfo[] = {
	DECLARE_PROPERTY( "ownerdraw",        T_BOOL,  0,                 "Owner draw",          0, \
                      ON_SET_ACTION_NONE, \
                      CODEGEN_ACTION_SET_STYLE( T("BS_OWNERDRAW")), \
                      ON_APPLY_ACTION( NONE) )
	DECLARE_PROPERTY( "auto",             T_BOOL,  1,                 "Auto",                0, \
                      ON_SET_ACTION_CUSTOM( PropOnSetCustomAction_Checkbox), \
                      CODEGEN_ACTION_CUSTOM( NULL), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "tristate",         T_BOOL,  0,                 "Tri-state",           0, \
                      ON_SET_ACTION_CUSTOM( PropOnSetCustomAction_Checkbox), \
                      CODEGEN_ACTION_CUSTOM( NULL), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "lefttext",         T_BOOL,  0,                 "Left text",           0, \
                      ON_SET_ACTION_SET_STYLE( BS_LEFTTEXT), \
                      CODEGEN_ACTION_SET_STYLE( T("BS_LEFTTEXT")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "bitmap",           T_BOOL,  0,                 "Bitmap",              0, \
                      ON_SET_ACTION_SET_STYLE( BS_ICON), \
                      CODEGEN_ACTION_SET_STYLE( T("BS_ICON")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "flat",             T_BOOL,  0,                 "Flat",                0, \
                      ON_SET_ACTION_SET_STYLE( BS_FLAT), \
                      CODEGEN_ACTION_SET_STYLE( T("BS_FLAT")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "multiline",        T_BOOL,  0,                 "Multi-line",          0, \
                      ON_SET_ACTION_SET_STYLE( BS_MULTILINE), \
                      CODEGEN_ACTION_SET_STYLE( T("BS_MULTILINE")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "pushlike",         T_BOOL,  0,                 "Push-like",           0, \
                      ON_SET_ACTION_SET_STYLE( BS_PUSHLIKE), \
                      CODEGEN_ACTION_SET_STYLE( T("BS_PUSHLIKE")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "notify",           T_BOOL,  0,                 "Notify",              0, \
                      ON_SET_ACTION_NONE, \
                      CODEGEN_ACTION_SET_STYLE( T("BS_NOTIFY")), \
                      ON_APPLY_ACTION( NONE) )
	DECLARE_PROPERTY( "vertalign",        T_LIST,  1,                 "Vertical align",     (UINT_PTR)button_vertalign_strings, \
                      ON_SET_ACTION_REM_STYLE_AND_SET_STYLELIST( BS_VCENTER, button_vertalign_styles), \
                      CODEGEN_ACTION_SET_STYLELIST( button_vertalign_styles_str), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "horalign",         T_LIST,  0,                 "Horizontal align",   (UINT_PTR)button_horalign_strings, \
                      ON_SET_ACTION_REM_STYLE_AND_SET_STYLELIST( BS_CENTER, button_horalign_styles), \
                      CODEGEN_ACTION_SET_STYLELIST( button_horalign_styles_str), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY_END
};


#define PropOnSetCustomAction_Radiobutton PropOnSetCustomAction_Button

PROPERTY_INFO radiobutton_propinfo[] = {
	DECLARE_PROPERTY( "ownerdraw",        T_BOOL,  0,                  "Owner draw",         0, \
                      ON_SET_ACTION_NONE, \
                      CODEGEN_ACTION_SET_STYLE( T("BS_OWNERDRAW")), \
                      ON_APPLY_ACTION( NONE) )
	DECLARE_PROPERTY( "auto",             T_BOOL,  1,                  "Auto",               0, \
                      ON_SET_ACTION_CUSTOM( PropOnSetCustomAction_Radiobutton), \
                      CODEGEN_ACTION_CUSTOM( NULL), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "lefttext",         T_BOOL,  0,                  "Left text",          0, \
                      ON_SET_ACTION_SET_STYLE( BS_LEFTTEXT), \
                      CODEGEN_ACTION_SET_STYLE( T("BS_LEFTTEXT")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "bitmap",           T_BOOL,  0,                  "Bitmap",             0, \
                      ON_SET_ACTION_SET_STYLE( BS_ICON), \
                      CODEGEN_ACTION_SET_STYLE( T("BS_ICON")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "flat",             T_BOOL,  0,                  "Flat",               0, \
                      ON_SET_ACTION_SET_STYLE( BS_FLAT), \
                      CODEGEN_ACTION_SET_STYLE( T("BS_FLAT")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "multiline",        T_BOOL,  0,                  "Multi-line",         0, \
                      ON_SET_ACTION_SET_STYLE( BS_MULTILINE), \
                      CODEGEN_ACTION_SET_STYLE( T("BS_MULTILINE")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "pushlike",         T_BOOL,  0,                  "Push-like",          0, \
                      ON_SET_ACTION_SET_STYLE( BS_PUSHLIKE), \
                      CODEGEN_ACTION_SET_STYLE( T("BS_PUSHLIKE")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "notify",           T_BOOL,  0,                  "Notify",             0, \
                      ON_SET_ACTION_NONE, \
                      CODEGEN_ACTION_SET_STYLE( T("BS_NOTIFY")), \
                      ON_APPLY_ACTION( NONE) )
	DECLARE_PROPERTY( "vertalign",        T_LIST,  1,                  "Vertical align",    (UINT_PTR)button_vertalign_strings, \
                      ON_SET_ACTION_REM_STYLE_AND_SET_STYLELIST( BS_VCENTER, button_vertalign_styles), \
                      CODEGEN_ACTION_SET_STYLELIST( button_vertalign_styles_str), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "horalign",         T_LIST,  0,                  "Horizontal align",  (UINT_PTR)button_horalign_strings, \
                      ON_SET_ACTION_REM_STYLE_AND_SET_STYLELIST( BS_CENTER, button_horalign_styles), \
                      CODEGEN_ACTION_SET_STYLELIST( button_horalign_styles_str), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY_END
};


TCHAR *edit_align_strings[] = {T("Left"), T("Center"), T("Right"), NULL};
DWORD edit_align_styles[] = {ES_LEFT, ES_CENTER, ES_RIGHT};
TCHAR *edit_align_styles_str[] = {T("ES_LEFT"), T("ES_CENTER"), T("ES_RIGHT")};

PROPERTY_INFO edit_propinfo[] = {
	DECLARE_PROPERTY( "autohscroll",      T_BOOL,  1,                  "Auto HScroll",       0, \
                      ON_SET_ACTION_SET_STYLE( ES_AUTOHSCROLL), \
                      CODEGEN_ACTION_SET_STYLE( T("ES_AUTOHSCROLL")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "autovscroll",      T_BOOL,  0,                  "Auto VScroll",       0, \
                      ON_SET_ACTION_SET_STYLE( ES_AUTOVSCROLL), \
                      CODEGEN_ACTION_SET_STYLE( T("ES_AUTOVSCROLL")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "lowercase",        T_BOOL,  0,                  "Lower case",         0, \
                      ON_SET_ACTION_SET_STYLE( ES_LOWERCASE), \
                      CODEGEN_ACTION_SET_STYLE( T("ES_LOWERCASE")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "multiline",        T_BOOL,  0,                  "Multi-line",         0, \
                      ON_SET_ACTION_SET_STYLE( ES_MULTILINE), \
                      CODEGEN_ACTION_SET_STYLE( T("ES_MULTILINE")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "nohidesel",        T_BOOL,  0,                  "No hide selection",  0, \
                      ON_SET_ACTION_SET_STYLE( ES_NOHIDESEL), \
                      CODEGEN_ACTION_SET_STYLE( T("ES_NOHIDESEL")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "number",           T_BOOL,  0,                  "Number only",        0, \
                      ON_SET_ACTION_SET_STYLE( ES_NUMBER), \
                      CODEGEN_ACTION_SET_STYLE( T("ES_NUMBER")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "oemconvert",       T_BOOL,  0,                  "OEM Convert",        0, \
                      ON_SET_ACTION_SET_STYLE( ES_OEMCONVERT), \
                      CODEGEN_ACTION_SET_STYLE( T("ES_OEMCONVERT")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "password",         T_BOOL,  0,                  "Password",           0, \
                      ON_SET_ACTION_SET_STYLE( ES_PASSWORD), \
                      CODEGEN_ACTION_SET_STYLE( T("ES_PASSWORD")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "readonly",         T_BOOL,  0,                  "Read-only",          0, \
                      ON_SET_ACTION_SET_STYLE( ES_READONLY), \
                      CODEGEN_ACTION_SET_STYLE( T("ES_READONLY")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "uppercase",        T_BOOL,  0,                  "Upper case",         0, \
                      ON_SET_ACTION_SET_STYLE( ES_UPPERCASE), \
                      CODEGEN_ACTION_SET_STYLE( T("ES_UPPERCASE")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "hscroll",          T_BOOL,  0,                  "Horizontal scroll",  0, \
                      ON_SET_ACTION_SET_STYLE( WS_HSCROLL), \
                      CODEGEN_ACTION_SET_STYLE( T("WS_HSCROLL")), \
                      ON_APPLY_ACTION( RECREATE) )
	DECLARE_PROPERTY( "vsroll",           T_BOOL,  0,                  "Vertical scroll",    0, \
                      ON_SET_ACTION_SET_STYLE( WS_VSCROLL), \
                      CODEGEN_ACTION_SET_STYLE( T("WS_VSCROLL")), \
                      ON_APPLY_ACTION( RECREATE) )
	DECLARE_PROPERTY( "align",            T_LIST,  0,                  "Alignment",          (UINT_PTR)edit_align_strings, \
                      ON_SET_ACTION_REM_STYLE_AND_SET_STYLELIST( (ES_RIGHT | ES_CENTER), edit_align_styles), \
                      CODEGEN_ACTION_SET_STYLELIST( edit_align_styles_str), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY_END
};

TCHAR *listbox_selection_strings[] = {T("No"), T("Standart"), T("Multiple"), T("Extended"), NULL};
DWORD listbox_selection_styles[] = {LBS_NOSEL, 0, LBS_MULTIPLESEL, LBS_EXTENDEDSEL};
TCHAR *listbox_selection_styles_str[] = {T("LBS_NOSEL"), NULL, T("LBS_MULTIPLESEL"), T("LBS_EXTENDEDSEL")};

PROPERTY_INFO listbox_propinfo[] = {
	DECLARE_PROPERTY( "disablenoscroll",  T_BOOL,  0,                  "Disable no scroll",  0, \
                      ON_SET_ACTION_SET_STYLE( LBS_DISABLENOSCROLL), \
                      CODEGEN_ACTION_SET_STYLE( T("LBS_DISABLENOSCROLL")), \
                      ON_APPLY_ACTION( RECREATE) )
	DECLARE_PROPERTY( "hasstrings",       T_BOOL,  0,                  "Has strings",        0, \
                      ON_SET_ACTION_SET_STYLE( LBS_HASSTRINGS), \
                      CODEGEN_ACTION_SET_STYLE( T("LBS_HASSTRINGS")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "multicolumn",      T_BOOL,  0,                  "Multi-column",       0, \
                      ON_SET_ACTION_SET_STYLE( LBS_MULTICOLUMN), \
                      CODEGEN_ACTION_SET_STYLE( T("LBS_MULTICOLUMN")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	/*DECLARE_PROPERTY( "nodata",           T_BOOL,  0,                  "No data",            0, \
                        ON_SET_ACTION_SET_STYLE( LBS_NODATA), \
                        CODEGEN_ACTION_SET_STYLE( T("LBS_NODATA")), \
                        ON_APPLY_ACTION( SETSTYLE) )*/
	DECLARE_PROPERTY( "nointegralheight", T_BOOL,  1,                  "No integral height", 0, \
                      ON_SET_ACTION_SET_STYLE( LBS_NOINTEGRALHEIGHT), \
                      CODEGEN_ACTION_SET_STYLE( T("LBS_NOINTEGRALHEIGHT")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "noredraw",         T_BOOL,  0,                  "No redraw",          0, \
                      ON_SET_ACTION_SET_STYLE( LBS_NOREDRAW), \
                      CODEGEN_ACTION_SET_STYLE( T("LBS_NOREDRAW")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "notify",           T_BOOL,  0,                  "Notify",             0, \
                      ON_SET_ACTION_NONE, \
                      CODEGEN_ACTION_SET_STYLE( T("LBS_NOTIFY")), \
                      ON_APPLY_ACTION( NONE) )
	DECLARE_PROPERTY( "sort",             T_BOOL,  0,                  "Sort",               0, \
                      ON_SET_ACTION_SET_STYLE( LBS_SORT), \
                      CODEGEN_ACTION_SET_STYLE( T("LBS_SORT")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "usetabstops",      T_BOOL,  0,                  "Use tab stops",      0, \
                      ON_SET_ACTION_SET_STYLE( LBS_USETABSTOPS), \
                      CODEGEN_ACTION_SET_STYLE( T("LBS_USETABSTOPS")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "wantkeyinput",     T_BOOL,  0,                  "Want key input",     0, \
                      ON_SET_ACTION_NONE, \
                      CODEGEN_ACTION_SET_STYLE( T("LBS_WANTKEYBOARDINPUT")), \
                      ON_APPLY_ACTION( NONE) )
	DECLARE_PROPERTY( "hscroll",          T_BOOL,  0,                  "Horizontal scroll",  0, \
                      ON_SET_ACTION_SET_STYLE( WS_HSCROLL), \
                      CODEGEN_ACTION_SET_STYLE( T("WS_HSCROLL")), \
                      ON_APPLY_ACTION( RECREATE) )
	DECLARE_PROPERTY( "vsroll",           T_BOOL,  1,                  "Vertical scroll",    0, \
                      ON_SET_ACTION_SET_STYLE( WS_VSCROLL), \
                      CODEGEN_ACTION_SET_STYLE( T("WS_VSCROLL")), \
                      ON_APPLY_ACTION( RECREATE) )
	DECLARE_PROPERTY( "selection",        T_LIST,  1,                  "Selection",          (UINT_PTR)listbox_selection_strings, \
                      ON_SET_ACTION_REM_STYLE_AND_SET_STYLELIST( (LBS_EXTENDEDSEL |  LBS_NOSEL |  LBS_MULTIPLESEL), listbox_selection_styles), \
                      CODEGEN_ACTION_SET_STYLELIST( listbox_selection_styles_str), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY_END
};


TCHAR *combobox_type_strings[] = {T("Simple"), T("Dropdown"), T("Dropdown list"), NULL};
DWORD combobox_type_styles[] = {CBS_SIMPLE, CBS_DROPDOWN, CBS_DROPDOWNLIST};
TCHAR *combobox_type_styles_str[] = {T("CBS_SIMPLE"), T("CBS_DROPDOWN"), T("CBS_DROPDOWNLIST")};

PROPERTY_INFO combobox_propinfo[] = {
	DECLARE_PROPERTY( "autohscroll",      T_BOOL,  1,                  "Auto HScroll",        0, \
                      ON_SET_ACTION_SET_STYLE( CBS_AUTOHSCROLL), \
                      CODEGEN_ACTION_SET_STYLE( T("CBS_AUTOHSCROLL")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "disablenoscroll",  T_BOOL,  0,                  "Disable no scroll",   0, \
                      ON_SET_ACTION_SET_STYLE( CBS_DISABLENOSCROLL), \
                      CODEGEN_ACTION_SET_STYLE( T("CBS_DISABLENOSCROLL")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "hasstrings",       T_BOOL,  0,                  "Has strings",         0, \
                      ON_SET_ACTION_SET_STYLE( CBS_HASSTRINGS), \
                      CODEGEN_ACTION_SET_STYLE( T("CBS_HASSTRINGS")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "lowercase",        T_BOOL,  0,                  "Lower case",          0, \
                      ON_SET_ACTION_SET_STYLE( CBS_LOWERCASE), \
                      CODEGEN_ACTION_SET_STYLE( T("CBS_LOWERCASE")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "nointegralheight", T_BOOL,  1,                  "No integral height",  0, \
                      ON_SET_ACTION_SET_STYLE( CBS_NOINTEGRALHEIGHT), \
                      CODEGEN_ACTION_SET_STYLE( T("CBS_NOINTEGRALHEIGHT")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "oemconvert",       T_BOOL,  0,                  "OEM Convert",         0, \
                      ON_SET_ACTION_SET_STYLE( CBS_OEMCONVERT), \
                      CODEGEN_ACTION_SET_STYLE( T("CBS_OEMCONVERT")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "sort",             T_BOOL,  0,                  "Sort",                0, \
                      ON_SET_ACTION_SET_STYLE( CBS_SORT), \
                      CODEGEN_ACTION_SET_STYLE( T("CBS_SORT")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "uppercase",        T_BOOL,  0,                  "Upper case",          0, \
                      ON_SET_ACTION_SET_STYLE( CBS_UPPERCASE), \
                      CODEGEN_ACTION_SET_STYLE( T("CBS_UPPERCASE")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY( "type",             T_LIST,  2,                  "Type",                (UINT_PTR)combobox_type_strings, \
                      ON_SET_ACTION_REM_STYLE_AND_SET_STYLELIST( CBS_DROPDOWNLIST, combobox_type_styles), \
                      CODEGEN_ACTION_SET_STYLELIST( combobox_type_styles_str), \
                      ON_APPLY_ACTION( RECREATE) )
	DECLARE_PROPERTY_END
};


TCHAR *static_type_strings[] = {T("Left text"), T("Center text"), T("Right text"), T("Icon"), T("Black rect"), T("Gray rect"), T("White rect"),
                                T("Black frame"), T("Gray frame"), T("White frame"), T("User item"), T("Simple"), T("No word wrap"),
                                T("Owner draw"), T("Bitmap"), T("Enhanced metafile"), T("Horizontal edge"), T("Vertical edge"), T("Frame edge"), NULL};
DWORD static_type_styles[] = {SS_LEFT, SS_CENTER, SS_RIGHT, SS_ICON, SS_BLACKRECT, SS_GRAYRECT, SS_WHITERECT,
                              SS_BLACKFRAME, SS_GRAYFRAME, SS_WHITEFRAME, SS_USERITEM, SS_SIMPLE, SS_LEFTNOWORDWRAP,
                              SS_OWNERDRAW, SS_BITMAP, SS_ENHMETAFILE, SS_ETCHEDHORZ, SS_ETCHEDVERT, SS_ETCHEDFRAME};
TCHAR *static_type_styles_str[] = {T("SS_LEFT"), T("SS_CENTER"), T("SS_RIGHT"), T("SS_ICON"), T("SS_BLACKRECT"), T("SS_GRAYRECT"), T("SS_WHITERECT"),
                                   T("SS_BLACKFRAME"), T("SS_GRAYFRAME"), T("SS_WHITEFRAME"), T("SS_USERITEM"), T("SS_SIMPLE"), T("SS_LEFTNOWORDWRAP"),
                                   T("SS_OWNERDRAW"), T("SS_BITMAP"), T("SS_ENHMETAFILE"), T("SS_ETCHEDHORZ"), T("SS_ETCHEDVERT"), T("SS_ETCHEDFRAME")};

PROPERTY_INFO static_propinfo[] = {
    DECLARE_PROPERTY( "type",             T_LIST,  0,                  "Type",                (UINT_PTR)static_type_strings, \
                        ON_SET_ACTION_REM_STYLE_AND_SET_STYLELIST( SS_TYPEMASK, static_type_styles), \
                        CODEGEN_ACTION_SET_STYLELIST( static_type_styles_str), \
                        ON_APPLY_ACTION( RECREATE) )
    DECLARE_PROPERTY( "centerimage",      T_BOOL,  0,                  "Center image",        0, \
                        ON_SET_ACTION_SET_STYLE( SS_CENTERIMAGE), \
                        CODEGEN_ACTION_SET_STYLE( T("SS_CENTERIMAGE")), \
                        ON_APPLY_ACTION( SETSTYLE) )
    DECLARE_PROPERTY( "editcontrol",      T_BOOL,  0,                  "Edit control",        0, \
                        ON_SET_ACTION_SET_STYLE( SS_EDITCONTROL), \
                        CODEGEN_ACTION_SET_STYLE( T("SS_EDITCONTROL")), \
                        ON_APPLY_ACTION( SETSTYLE) )
    DECLARE_PROPERTY( "endellipsis",      T_BOOL,  0,                  "End ellipsis",        0, \
                        ON_SET_ACTION_SET_STYLE( SS_ENDELLIPSIS), \
                        CODEGEN_ACTION_SET_STYLE( T("SS_ENDELLIPSIS")), \
                        ON_APPLY_ACTION( SETSTYLE) )
    DECLARE_PROPERTY( "noprefix",         T_BOOL,  0,                  "No prefix",           0, \
                        ON_SET_ACTION_SET_STYLE( SS_NOPREFIX), \
                        CODEGEN_ACTION_SET_STYLE( T("SS_NOPREFIX")), \
                        ON_APPLY_ACTION( SETSTYLE) )
    DECLARE_PROPERTY( "notify",           T_BOOL,  1,                  "Notify",              0, \
                        ON_SET_ACTION_NONE, \
                        CODEGEN_ACTION_SET_STYLE( T("SS_NOTIFY")), \
                        ON_APPLY_ACTION( NONE) )
    DECLARE_PROPERTY( "pathellipsis",     T_BOOL,  0,                  "Path ellipsis",       0, \
                        ON_SET_ACTION_SET_STYLE( SS_PATHELLIPSIS), \
                        CODEGEN_ACTION_SET_STYLE( T("SS_PATHELLIPSIS")), \
                        ON_APPLY_ACTION( SETSTYLE) )
    DECLARE_PROPERTY( "realsizecontrol",  T_BOOL,  0,                  "Real size control",   0, \
                        ON_SET_ACTION_SET_STYLE( SS_REALSIZECONTROL), \
                        CODEGEN_ACTION_SET_STYLE( T("SS_REALSIZECONTROL")), \
                        ON_APPLY_ACTION( SETSTYLE) )
    DECLARE_PROPERTY( "realsizeimage",    T_BOOL,  0,                  "Real size image",     0, \
                        ON_SET_ACTION_SET_STYLE( SS_REALSIZEIMAGE), \
                        CODEGEN_ACTION_SET_STYLE( T("SS_REALSIZEIMAGE")), \
                        ON_APPLY_ACTION( SETSTYLE) )
    DECLARE_PROPERTY( "rightjust",        T_BOOL,  0,                  "Right just",          0, \
                        ON_SET_ACTION_SET_STYLE( SS_RIGHTJUST), \
                        CODEGEN_ACTION_SET_STYLE( T("SS_RIGHTJUST")), \
                        ON_APPLY_ACTION( SETSTYLE) )
    DECLARE_PROPERTY( "sunken",           T_BOOL,  0,                  "Sunken border",       0, \
                        ON_SET_ACTION_SET_STYLE( SS_SUNKEN), \
                        CODEGEN_ACTION_SET_STYLE( T("SS_SUNKEN")), \
                        ON_APPLY_ACTION( RECREATE) )
    DECLARE_PROPERTY( "wordellipsis",     T_BOOL,  0,                  "Word ellipsis",       0, \
                        ON_SET_ACTION_SET_STYLE( SS_WORDELLIPSIS), \
                        CODEGEN_ACTION_SET_STYLE( T("SS_WORDELLIPSIS")), \
                        ON_APPLY_ACTION( SETSTYLE) )
    DECLARE_PROPERTY_END
};


PROPERTY_INFO groupbox_propinfo[] = {
    DECLARE_PROPERTY( "horalign",         T_LIST,  0,                  "Horizontal align",    (UINT_PTR)button_horalign_strings, \
                        ON_SET_ACTION_REM_STYLE_AND_SET_STYLELIST( BS_CENTER, button_horalign_styles), \
                        CODEGEN_ACTION_SET_STYLELIST( button_horalign_styles_str), \
                        ON_APPLY_ACTION( SETSTYLE) )
    DECLARE_PROPERTY_END
};


TCHAR *slider_ticks_strings[] = {T("None"), T("Bottom/Right"), T("Top/Left"), T("Both"), NULL};
DWORD slider_ticks_styles[] = {TBS_NOTICKS, TBS_BOTTOM, TBS_TOP, TBS_BOTH};
TCHAR *slider_ticks_styles_str[] = {T("TBS_NOTICKS"), T("TBS_BOTTOM"), T("TBS_TOP"), T("TBS_BOTH")};

PROPERTY_INFO slider_propinfo[] = {
    DECLARE_PROPERTY( "autoticks",        T_BOOL,  0,                  "Auto ticks",          0, \
                        ON_SET_ACTION_SET_STYLE( TBS_AUTOTICKS), \
                        CODEGEN_ACTION_SET_STYLE( T("TBS_AUTOTICKS")), \
                        ON_APPLY_ACTION( SETSTYLE) )
    DECLARE_PROPERTY( "enableselrange",   T_BOOL,  0,                  "Enable sel range",    0, \
                        ON_SET_ACTION_SET_STYLE( TBS_ENABLESELRANGE), \
                        CODEGEN_ACTION_SET_STYLE( T("TBS_ENABLESELRANGE")), \
                        ON_APPLY_ACTION( SETSTYLE) )
    DECLARE_PROPERTY( "nothumb",          T_BOOL,  0,                  "No thumb",            0, \
                        ON_SET_ACTION_SET_STYLE( TBS_NOTHUMB), \
                        CODEGEN_ACTION_SET_STYLE( T("TBS_NOTHUMB")), \
                        ON_APPLY_ACTION( SETSTYLE) )
    DECLARE_PROPERTY( "ticks",           T_LIST,  1,                   "Ticks",               (UINT_PTR)slider_ticks_strings, \
                        ON_SET_ACTION_REM_STYLE_AND_SET_STYLELIST( TBS_NOTICKS | TBS_TOP | TBS_BOTH, slider_ticks_styles), \
                        CODEGEN_ACTION_SET_STYLELIST( slider_ticks_styles_str), \
                        ON_APPLY_ACTION( SETSTYLE) )
    DECLARE_PROPERTY( "tooltips",         T_BOOL,  0,                  "Tooltips",            0, \
                        ON_SET_ACTION_SET_STYLE( TBS_TOOLTIPS), \
                        CODEGEN_ACTION_SET_STYLE( T("TBS_TOOLTIPS")), \
                        ON_APPLY_ACTION( SETSTYLE) )
    DECLARE_PROPERTY( "vertical",         T_BOOL,  0,                  "Vertical",            0, \
                        ON_SET_ACTION_SET_STYLE( TBS_VERT), \
                        CODEGEN_ACTION_SET_STYLE( T("TBS_VERT")), \
                        ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY_END
};


PROPERTY_INFO progressbar_propinfo[] = {
    DECLARE_PROPERTY( "marquee",          T_BOOL,  0,                  "Marquee",             0, \
                        ON_SET_ACTION_SET_STYLE( PBS_MARQUEE), \
                        CODEGEN_ACTION_SET_STYLE( T("PBS_MARQUEE")), \
                        ON_APPLY_ACTION( SETSTYLE) )
    DECLARE_PROPERTY( "smooth",           T_BOOL,  0,                  "Smooth",              0, \
                        ON_SET_ACTION_SET_STYLE( PBS_SMOOTH), \
                        CODEGEN_ACTION_SET_STYLE( T("PBS_SMOOTH")), \
                        ON_APPLY_ACTION( SETSTYLE) )
    DECLARE_PROPERTY( "smoothreverse",    T_BOOL,  0,                  "Smooth reverse",       0, \
                        ON_SET_ACTION_SET_STYLE( PBS_SMOOTHREVERSE), \
                        CODEGEN_ACTION_SET_STYLE( T("PBS_SMOOTHREVERSE")), \
                        ON_APPLY_ACTION( SETSTYLE) )
    DECLARE_PROPERTY( "vertical",         T_BOOL,  0,                  "Vertical",             0, \
                        ON_SET_ACTION_SET_STYLE( PBS_VERTICAL), \
                        CODEGEN_ACTION_SET_STYLE( T("PBS_VERTICAL")), \
                        ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY_END
};


PROPERTY_INFO ipaddress_propinfo[] = {
	DECLARE_PROPERTY_END
};


TCHAR *datetime_format_strings[] = {T("Short"), T("Short with century"), T("Long"), T("Time"), NULL};
DWORD datetime_format_styles[] = {DTS_SHORTDATEFORMAT, DTS_SHORTDATECENTURYFORMAT, DTS_LONGDATEFORMAT, DTS_TIMEFORMAT};
TCHAR *datetime_format_styles_str[] = {T("DTS_SHORTDATEFORMAT"), T("DTS_SHORTDATECENTURYFORMAT"), T("DTS_LONGDATEFORMAT"), T("DTS_TIMEFORMAT")};

PROPERTY_INFO datetime_propinfo[] = {
    DECLARE_PROPERTY( "appcanparse",      T_BOOL,  0,                  "App can parse",        0, \
                        ON_SET_ACTION_SET_STYLE( DTS_APPCANPARSE), \
                        CODEGEN_ACTION_SET_STYLE( T("DTS_APPCANPARSE")), \
                        ON_APPLY_ACTION( SETSTYLE) )
    DECLARE_PROPERTY( "format",           T_LIST,  0,                  "Format",               (UINT_PTR)datetime_format_strings, \
                        ON_SET_ACTION_REM_STYLE_AND_SET_STYLELIST( DTS_SHORTDATEFORMAT | DTS_SHORTDATECENTURYFORMAT |
                                                                   DTS_LONGDATEFORMAT | DTS_TIMEFORMAT, datetime_format_styles), \
                        CODEGEN_ACTION_SET_STYLELIST( datetime_format_styles_str), \
                        ON_APPLY_ACTION( SETSTYLE) )
    DECLARE_PROPERTY( "rightalign",       T_BOOL,  0,                  "Right align",          0, \
                        ON_SET_ACTION_SET_STYLE( DTS_RIGHTALIGN), \
                        CODEGEN_ACTION_SET_STYLE( T("DTS_RIGHTALIGN")), \
                        ON_APPLY_ACTION( SETSTYLE) )
    DECLARE_PROPERTY( "shownone",         T_BOOL,  0,                  "Show none",            0, \
                        ON_SET_ACTION_SET_STYLE( DTS_SHOWNONE), \
                        CODEGEN_ACTION_SET_STYLE( T("DTS_SHOWNONE")), \
                        ON_APPLY_ACTION( SETSTYLE) )
    DECLARE_PROPERTY( "updown",           T_BOOL,  0,                  "Up-Down",              0, \
                        ON_SET_ACTION_SET_STYLE( DTS_UPDOWN), \
                        CODEGEN_ACTION_SET_STYLE( T("DTS_UPDOWN")), \
                        ON_APPLY_ACTION( SETSTYLE) )
	DECLARE_PROPERTY_END
};

PROPERTY_INFO scrollbar_propinfo[] = {
	DECLARE_PROPERTY_END
};

PROPERTY_INFO updown_propinfo[] = {
	DECLARE_PROPERTY_END
};

PROPERTY_INFO statusbar_propinfo[] = {
	DECLARE_PROPERTY_END
};

PROPERTY_INFO toolbar_propinfo[] = {
	DECLARE_PROPERTY( "list",             T_BOOL,  0,                  "List",                0, \
                      ON_SET_ACTION_SET_STYLE( TBSTYLE_LIST), \
                      CODEGEN_ACTION_SET_STYLE( T("TBSTYLE_LIST")), \
                      ON_APPLY_ACTION( SETSTYLE) )
	/*DECLARE_PROPERTY( "right",            T_BOOL,  0,                  "Right",               0, \
                        ON_SET_ACTION_SET_STYLE( CCS_RIGHT), \
                        CODEGEN_ACTION_SET_STYLE( T("CCS_RIGHT")), \
                      ON_APPLY_ACTION( SETSTYLE) )*/
	DECLARE_PROPERTY_END
};


///////////////////
// Class defines //
///////////////////

#define WINDOW_CLASS        T("DefaultWnd")
#define BUTTON_CLASS        WC_BUTTON
#define CHECKBOX_CLASS      WC_BUTTON
#define RADIOBUTTON_CLASS   WC_BUTTON
#define EDIT_CLASS          WC_EDIT
#define LISTBOX_CLASS       WC_LISTBOX
#define COMBOBOX_CLASS      WC_COMBOBOX
#define STATIC_CLASS        WC_STATIC
#define GROUPBOX_CLASS      WC_BUTTON
#define SLIDER_CLASS        TRACKBAR_CLASS
#define PROGRESSBAR_CLASS   PROGRESS_CLASS
#define IPADDRESS_CLASS     WC_IPADDRESS
#define DATETIME_CLASS      DATETIMEPICK_CLASS
#define SCROLLBAR_CLASS     WC_SCROLLBAR
#ifndef UPDOWN_CLASS
#define UPDOWN_CLASS        UPDOWN_CLASS
#endif
#define STATUSBAR_CLASS     STATUSCLASSNAME
#define TOOLBAR_CLASS       TOOLBARCLASSNAME

//#define RICHEDIT_CLASS    MSFTEDIT_CLASS


#define DECLARE_CTRL_INFO(nameh, namel, nameobj, codegen_act) \
        { nameh##_CLASS, T("TWC_")T(#nameh)T("_CLASS"), nameh##_PROPERTIES_COUNT, namel##_propinfo, T(#nameobj), codegen_act }
#define DECLARE_CTRL_INFO_END { NULL, NULL, 0, NULL, NULL }

CONTROL_INFO control_info[CONTROL_COUNT] = {
    DECLARE_CTRL_INFO( WINDOW,       window,       Window,      NULL                           ), /* WINDOW */
    DECLARE_CTRL_INFO( BUTTON,       button,       Button,      NULL                           ), /* BUTTON */
    DECLARE_CTRL_INFO( CHECKBOX,     checkbox,     Checkbox,    PropCodegenCustomAction_Button ), /* CHECKBOX */
    DECLARE_CTRL_INFO( RADIOBUTTON,  radiobutton,  Radiobutton, PropCodegenCustomAction_Button ), /* RADIOBUTTON */
    DECLARE_CTRL_INFO( EDIT,         edit,         Edit,        NULL                           ), /* EDIT */
    DECLARE_CTRL_INFO( LISTBOX,      listbox,      Listbox,     NULL                           ), /* LISTBOX */
    DECLARE_CTRL_INFO( COMBOBOX,     combobox,     Combobox,    NULL                           ), /* COMBOBOX */
    DECLARE_CTRL_INFO( STATIC,       static,       Static,      NULL                           ), /* STATIC */
    DECLARE_CTRL_INFO( GROUPBOX,     groupbox,     Groupbox,    NULL                           ), /* GROUPBOX */
    DECLARE_CTRL_INFO( SLIDER,       slider,       Slider,      NULL                           ), /* SLIDER */
    DECLARE_CTRL_INFO( PROGRESSBAR,  progressbar,  Progressbar, NULL                           ), /* PROGRESSBAR */
    DECLARE_CTRL_INFO( IPADDRESS,    ipaddress,    IPAddress,   NULL                           ), /* IPADDRESS */
    DECLARE_CTRL_INFO( DATETIME,     datetime,     Datetime,    NULL                           ), /* DATETIME */
    DECLARE_CTRL_INFO( SCROLLBAR,    scrollbar,    Scrollbar,   NULL                           ), /* SCROLLBAR */
    DECLARE_CTRL_INFO( UPDOWN,       updown,       Updown,      NULL                           ), /* UPDOWN */
    DECLARE_CTRL_INFO( STATUSBAR,    statusbar,    Statusbar,   NULL                           ), /* STATUSBAR */
    DECLARE_CTRL_INFO( TOOLBAR,      toolbar,      Toolbar,     NULL                           ), /* TOOLBAR */
    DECLARE_CTRL_INFO_END
};



CONTROL_INFO *GetControlInfo( UINT ctrl_id)
{
    TWC_CHECKIT( ctrl_id < CONTROL_LAST_ID && ctrl_id != CTRL_ID_UNDEFINED);

    return &control_info[ctrl_id];
}

PROPERTY_INFO *GetPropertyInfo( UINT ctrl_id, UINT prop_id)
{
    TWC_CHECKIT( ctrl_id < CONTROL_LAST_ID && ctrl_id != CTRL_ID_UNDEFINED);
    TWC_CHECKIT( prop_id != PROPERTIES_ALL );
    TWC_CHECKIT( prop_id < GetControlPropertiesCount( ctrl_id) );

    //Common properties
    if ( prop_id < COMMON_PROPERTIES_END )
        return &common_propinfo[prop_id];

    //Control-specific properties
    prop_id -= COMMON_PROPERTIES_END;
    
    return &control_info[ctrl_id].property_info[prop_id];
}

UINT GetControlPropertiesCount( UINT ctrl_id)
{
    TWC_CHECKIT( ctrl_id < CONTROL_LAST_ID && ctrl_id != CTRL_ID_UNDEFINED);

    return control_info[ctrl_id].properties_count;
}

TCHAR *GetControlClassname( UINT ctrl_id)
{
    TWC_CHECKIT( ctrl_id < CONTROL_LAST_ID && ctrl_id != CTRL_ID_UNDEFINED);

    return control_info[ctrl_id].class_name;
}

TCHAR *GetControlClassnameTWC( UINT ctrl_id)
{
    TWC_CHECKIT( ctrl_id < CONTROL_LAST_ID && ctrl_id != CTRL_ID_UNDEFINED);

    return control_info[ctrl_id].class_name_twc;
}

TCHAR *GetControlDefaultObjectName( UINT ctrl_id)
{
    TWC_CHECKIT( ctrl_id < CONTROL_LAST_ID && ctrl_id != CTRL_ID_UNDEFINED);

    return control_info[ctrl_id].def_obj_name;
}

PROP_CODEGEN_CUSTOM_ACT_FUNC GetControlCodegenCustomAct( UINT ctrl_id)
{
    TWC_CHECKIT( ctrl_id < CONTROL_LAST_ID && ctrl_id != CTRL_ID_UNDEFINED);

    return control_info[ctrl_id].codegen_custom_act;
}

UINT GetPropertyIdByName( TCHAR *name, UINT ctrl_id)
{
    PROPERTY_INFO *propinfo;
    UINT prop_id;
    UINT prop_count;

    for ( prop_id = COMMON_PROPERTIES_BEGIN, propinfo = common_propinfo;
          prop_id < COMMON_PROPERTIES_END;
          prop_id++, propinfo++ ) {
        if ( _tcscmp( name, propinfo->name) == 0 ) {
            return prop_id;
        }
    }

    prop_count = GetControlPropertiesCount( ctrl_id);
    for ( prop_id = COMMON_PROPERTIES_END, propinfo = GetPropertyInfo( ctrl_id, COMMON_PROPERTIES_END);
          prop_id < prop_count;
          prop_id++, propinfo++ ) {
        if ( _tcscmp( name, propinfo->name) == 0 ) {
            return prop_id;
        }
    }

    return PROPERTY_UNDEFINED;
}