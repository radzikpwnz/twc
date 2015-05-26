#include <windows.h>
#include "twc.h"
#include "interface.h"

//////////////////
// Main Window ///
//////////////////

TWC_SOBJECT txtLog = {
	TWC_EDIT_CLASS,
	5, 5,
	284, 180,
	NULL,
	WS_BORDER | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL
};

TWC_SOBJECT MDIClient = {
	TWC_MDICLIENT_CLASS,
	0, 0,
	550, 550,
	NULL,
	WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
	WS_EX_COMPOSITED
};

TWC_SOBJECT Toolbox = {
	TWC_TOOLBAR_CLASS,
	0, 0,
	0, 0,
	NULL,
	WS_VISIBLE | TBSTYLE_LIST | TBSTYLE_FLAT | WS_CLIPSIBLINGS | CCS_RIGHT | CCS_NODIVIDER | CCS_NORESIZE | WS_BORDER
	//WS_EX_COMPOSITED
};

TWC_SOBJECT MainToolbar = {
	TWC_TOOLBAR_CLASS,
	0, 0,
	0, 0,
	NULL,
	WS_VISIBLE | TBSTYLE_FLAT | WS_CLIPSIBLINGS | CCS_NODIVIDER | CCS_NORESIZE | WS_BORDER | TBSTYLE_TOOLTIPS
	//WS_EX_COMPOSITED
};


TWC_SOBJECT StatusBar = {
	TWC_STATUSBAR_CLASS,
	0, 0,
	0, 0,
	NULL,
	WS_VISIBLE |  WS_CLIPSIBLINGS
};

TWC_SOBJECT *MainWnd_controls[] = {&MDIClient, &Toolbox, &StatusBar, &MainToolbar, NULL};

TWC_SOBJECT MainWnd = {
	TEXT("MainWnd"),
	CW_USEDEFAULT, 0,
	1200, 750,
	TEXT("TWC Design"),
	WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
	0,/*WS_EX_APPWINDOW,*/
    0,
	MainWnd_controls
};

////////////////////////////


//////////////////////////
//// About Window////
/////////////////////////

TWC_SOBJECT AboutWnd_lblTitle = {
	TWC_STATIC_CLASS,
	0, 8,
	150, 20,
	TEXT("TWC Design"),
	WS_VISIBLE | SS_CENTER,
};

TWC_SOBJECT AboutWnd_lblVersion = {
	TWC_STATIC_CLASS,
	0, 28,
	150, 20,
	TEXT("v1.0 beta 2"),
	WS_VISIBLE | SS_CENTER,
};

TWC_SOBJECT AboutWnd_lblByRadzik = {
	TWC_STATIC_CLASS,
	0, 137,
	150, 20,
	TEXT("by R@DZIK"),
	WS_VISIBLE | SS_CENTER,
};

TWC_SOBJECT AboutWnd_btnViewChanges = {
	TWC_BUTTON_CLASS,
	25, 46,
	100, 20,
	TEXT("View changes"),
	WS_VISIBLE,
};

TWC_SOBJECT AboutWnd_btnSendFeedback = {
	TWC_BUTTON_CLASS,
	25, 67,
	100, 20,
	TEXT("Send Feedback"),
	WS_VISIBLE | WS_DISABLED,
};

TWC_SOBJECT AboutWnd_btnClose = {
	TWC_BUTTON_CLASS,
	35, 110,
	80, 20,
	TEXT("Close"),
	WS_VISIBLE,
};

TWC_SOBJECT *AboutWnd_controls[] = {&AboutWnd_lblTitle, &AboutWnd_lblVersion, &AboutWnd_lblByRadzik, &AboutWnd_btnViewChanges, &AboutWnd_btnSendFeedback, &AboutWnd_btnClose, NULL};

TWC_SOBJECT AboutWnd = {
	TEXT("AboutWnd"),
	0, 0,
	150, 180,
	TEXT("About"),
	WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
	WS_EX_TOOLWINDOW | WS_EX_DLGMODALFRAME,
    0,
	AboutWnd_controls
};