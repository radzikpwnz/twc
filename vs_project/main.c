#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS
#define _USE_MATH_DEFINES

#include <windows.h>
#include <commctrl.h>

#include "twc_design.h"

#include "stuff.h"
#include "resource.h"
#include "twc.h"
#include "interface.h"
#include "..\property_list\proplist.h"
#include "toolbox.h"
#include "toolbars.h"
#include "mainwnd.h"
#include "window.h"
#include "static.h"

/*
#define VLD_FORCE_ENABLE
#include "vld.h"
*/

/**
 * Global variables
 */
int new_control = 0;       /* Flag indicates that new control is being created from toolbox */
int grid_size = 2;         /* Grid size */
RT_OBJECT *current_object; /* Pointer to current object info */
HWND hMainWnd;             /* Main window handle */
HWND hPropList;            /* Property list handle */

/**
 * Create property list.
 */
static int CreatePropList()
{
    TWC_CHECKIT( hMainWnd != NULL );

    hPropList = CreateWindowEx( 0, GetPropListClassname(), NULL,
                                WS_BORDER | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VSCROLL,
                                0, 0, 150, 0, hMainWnd, NULL, GetModuleHandle( NULL), NULL);
    if ( hPropList == NULL ) {
        return 0;
    }

    /* Setting z-order(top), item height and font */
	SetWindowPos( hPropList, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	SendMessage( hPropList, PL_SETITEMHEIGHT, 20, 0);
	SendMessage( hPropList, WM_SETFONT, (WPARAM)GetStockObject( DEFAULT_GUI_FONT), 0);

    return 1;
}

/**
 * Register window classes.
 */
static int RegisterWindowClasses()
{
    static WNDCLASSEX wincl = { sizeof (WNDCLASSEX), 0, MainWndProc, 0, 0, 0, 0, 0,
                                (HBRUSH)(COLOR_BTNFACE + 1), MAKEINTRESOURCE(IDR_MENU1), T("MainWnd"), 0 };

    /* Main Window */
	wincl.hCursor = LoadCursor( NULL, IDC_ARROW);
	if ( !RegisterClassEx( &wincl) ) return 0;

	/* Default Window */
	wincl.style = CS_DBLCLKS;
	wincl.lpszMenuName = NULL;
	wincl.lpszClassName = GetControlClassname( CTRL_ID_WINDOW);
	wincl.lpfnWndProc = DefWindowProc;
	if ( !RegisterClassEx( &wincl) ) return 0;

	/* Code window */
	wincl.style = 0;
	wincl.lpszMenuName = NULL;
	wincl.lpszClassName = T("CodeWnd");
	wincl.lpfnWndProc = CodeWndProc;
	if ( !RegisterClassEx( &wincl) ) return 0;

    return 1;
}

/**
 * Program initialization.
 */
static int Init()
{
    /* Load control cursors */
	LoadControlCursors();

    /* Register window classes */
    if ( RegisterWindowClasses() == 0 ) {
        return 0;
    }

    /* Initialize common controls library */
    InitCommonControlsEx( NULL);

    /* Initialize property list dll */
	if ( InitPropertyListDll( GetModuleHandle(NULL)) == 0 ) {
        return 0;
    }

    /* Initialize TWC and creating main window */
    if ( !twcInit() || !(twcCreateObject( &MainWnd, 1, HWND_DESKTOP)) ) {
        return 0;
    }
	hMainWnd = MainWnd.hwnd;

    /* Create property list */
    if ( CreatePropList() == 0) {
        return 0;
    }

    /* Set status bar z-order(top) */
    SetWindowPos( StatusBar.hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    /* Add toolbox and toolbar buttons */
    AddToolboxButtons(Toolbox.hwnd);
	AddMainToolbarButtons(MainToolbar.hwnd);

    /* Show main window */
    ShowWindow( hMainWnd, SW_SHOWNORMAL);
	UpdateWindow( hMainWnd);
	SetStatusText( T("Ready!"));

    return 1;
}

/**
 * Program entry point.
 */
int CALLBACK WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;

    /* Initialization */
    if ( Init() == 0 ) {
        MessageBox( HWND_DESKTOP, T("Error during initialization!"), T("Error"), 0);
    }

    /* Main loop */
	while ( GetMessage( &msg, NULL, 0, 0)) {
		TranslateMessage( &msg);
		DispatchMessage( &msg);
	}
	ExitProcess( msg.wParam);
}
