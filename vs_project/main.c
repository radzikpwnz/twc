#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS
#define _USE_MATH_DEFINES

#include <windows.h>
#include <commctrl.h>

#include "common.h"

#include "stuff.h"
#include "resource.h"
#include "twc.h"
#include "interface.h"
#include "..\property_list\proplist.h"
#include "toolbox.h"
#include "toolbars.h"
#include "mainwnd.h"
#include "window.h"

//#define VLD_FORCE_ENABLE
//#include "vld.h"

//void AddLog(const TCHAR *text);

//Windows params
HWND hMainWnd;
WNDCLASSEX wincl = {sizeof (WNDCLASSEX), /*CS_HREDRAW | CS_VREDRAW*/0, MainWndProc, 0, 0, 0, 0, 0, (HBRUSH)(COLOR_BTNFACE + 1), MAKEINTRESOURCE(IDR_MENU1), T("MainWnd"), 0};

int new_control = 0;
int grid_size = 2;

RT_OBJECT *current_object;

HWND hPropList;

HMODULE msft_mod;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
//int _WinMain()
{
	MSG msg;
	PL_ITEMINFO iteminfo;
	/*RECT rect;
	HWND hTmp;*/

	LoadControlCursors();

	//Main Window
	wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
	if (!RegisterClassEx (&wincl)) return 0;

	//Default Window
	wincl.style = CS_DBLCLKS;
	wincl.lpszMenuName = NULL;
	wincl.lpszClassName = GetControlClassname( CTRL_ID_WINDOW);
	wincl.lpfnWndProc = DefWindowProc;
	if (!RegisterClassEx (&wincl)) return 0;

	//Code window
	wincl.style = 0;
	wincl.lpszMenuName = NULL;
	wincl.lpszClassName = T("CodeWnd");
	wincl.lpfnWndProc = CodeWndProc;
	if (!RegisterClassEx (&wincl)) return 0;

	InitCommonControlsEx(NULL);
	if (InitPropertyListDll(GetModuleHandle(NULL)) == 0) return 0;

	if (!twcInit() || !(twcCreateObject(&MainWnd, 1, HWND_DESKTOP))) return 0;
	hMainWnd = MainWnd.hwnd;

	hPropList = CreateWindowEx(0, GetPropListClassname(), NULL, WS_BORDER | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VSCROLL, 0, 0, 150, 0, hMainWnd, NULL, GetModuleHandle(NULL), NULL);
	SetWindowPos(hPropList, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	SetWindowPos(StatusBar.hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	iteminfo.id = 0;
	/*hTmp = CreateWindowEx(0, WC_COMBOBOX, NULL, WS_CHILD | CBS_DROPDOWN, 0, 0, 0, 0, hMainWnd, NULL, GetModuleHandle(NULL), NULL);
	GetClientRect(hTmp, &rect);
	DestroyWindow(hTmp);*/

	SendMessage(hPropList, PL_SETITEMHEIGHT, 20, 0);
	SendMessage(hPropList, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), 0);

	AddToolboxButtons(Toolbox.hwnd);
	AddMainToolbarButtons(MainToolbar.hwnd);

	ShowWindow(hMainWnd, SW_SHOWNORMAL);
	UpdateWindow(hMainWnd);
	SetStatusText(T("Ready!"));

	msft_mod = LoadLibrary(T("Msftedit.dll"));
	LoadLibrary(T("Riched20.dll"));
	if (msft_mod == NULL) {
		MessageBox(hMainWnd, T("Unable to load Msftedit.dll, some code edit functions will be disabled!"), T("Warning!"), 0);
	}

	while (GetMessage (&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	ExitProcess(msg.wParam);
}

/*void AddLog(const TCHAR *text)
{
	register int n;
	
	n = GetWindowTextLength(txtLog.hwnd);
	SendMessage(txtLog.hwnd, EM_SETSEL, (WPARAM)n, (LPARAM)n);
	SendMessage(txtLog.hwnd, EM_REPLACESEL, 0, (LPARAM) text); 
	return;
}*/
