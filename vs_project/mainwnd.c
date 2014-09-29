#include <windows.h>
#include <tchar.h>

#include "twc_design.h"

#include "il.h"
#include "codegen.h"
#include "window.h"
#include "object.h"
#include "win_stuff.h"
#include "interface.h"
#include "toolbox.h"
#include "..\property_list\proplist.h"
#include "toolbars.h"
#include "proplst.h"

#include "resource.h"

#include "mainwnd.h"


#define MAX_TOOLBAR_SIZE 230
#define MAINTOOLBAR_HEIGHT 28


void Resize(int width, int height);


RECT mainwnd_rect;


BOOL CALLBACK AboutDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int lEvent = LOWORD(wParam);
	LOGFONT lf;
	RECT rect, rect1;
	HDC hDC;
	PAINTSTRUCT ps;
	static HFONT hFont1, hFont2, hFont3;

    switch(msg) {
		case WM_INITDIALOG:
            GetWindowRect( GetParent( hwnd), &rect);
            GetClientRect( hwnd, &rect1);
            SetWindowPos( hwnd, NULL, (rect.right + rect.left) / 2 - (rect1.right - rect1.left) / 2,
                                      (rect.bottom + rect.top) / 2 - (rect1.bottom - rect1.top) / 2,
                                      0, 0, SWP_NOZORDER | SWP_NOSIZE);

			GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);

			_tcscpy(lf.lfFaceName, TEXT("Impact"));
			lf.lfHeight = 22;
			lf.lfOutPrecision = OUT_TT_ONLY_PRECIS;
			lf.lfQuality = CLEARTYPE_NATURAL_QUALITY;
			lf.lfWeight = FW_NORMAL;
			hFont1 = CreateFontIndirect(&lf);
			SendMessage(GetDlgItem(hwnd, IDC_AboutWnd_lblTitle), WM_SETFONT, (WPARAM)hFont1, 1);

			_tcscpy(lf.lfFaceName, TEXT("Tahoma"));
			lf.lfHeight = 12;
			hFont2 = CreateFontIndirect(&lf);
			SendMessage(GetDlgItem(hwnd, IDC_AboutWnd_lblVersion), WM_SETFONT, (WPARAM)hFont2, 1);

			_tcscpy(lf.lfFaceName, TEXT("Arial Narrow"));
			lf.lfHeight = 18;
			lf.lfItalic = 1;
			lf.lfWeight = FW_BOLD;
			hFont3 = CreateFontIndirect(&lf);
			SendMessage(GetDlgItem(hwnd, IDC_AboutWnd_lblByRadzik), WM_SETFONT, (WPARAM)hFont3, 1);

            SetWindowText( GetDlgItem( hwnd, IDC_AboutWnd_lblVersion), GetProgramVersion());
			break;
		case WM_COMMAND:
			switch (lEvent) {
		        case IDC_AboutWnd_btnClose:
					EndDialog(hwnd, (INT_PTR)NULL);
					break;
				case IDC_AboutWnd_btnViewChanges:
					SetCurrentDirToExeFolder();
					ShellExecute(hwnd, NULL, T("history.txt"), NULL, NULL, SW_SHOWDEFAULT);
					break;
                case IDC_AboutWnd_btnSendFeedback:
                    ShellExecute(hwnd, NULL, T("mailto:radzikpwnz@gmail.com"), NULL, NULL, SW_SHOWDEFAULT);
					break;
			}
			break;
		case WM_CLOSE:
			EndDialog(hwnd, (INT_PTR)NULL);
			break;
		case WM_DESTROY:
			DeleteObject(hFont1);
			DeleteObject(hFont2);
			DeleteObject(hFont3);
			break;
	  case WM_PAINT:
			hDC = BeginPaint(hwnd, &ps);
			GetClientRect(hwnd, &rect);
			GetWindowRect(GetDlgItem(hwnd, IDC_AboutWnd_lblByRadzik), &rect1);
			MapWindowPoints(HWND_DESKTOP, hwnd, (POINT *)&rect1, 2);
			MoveToEx(hDC, 0, rect1.top - 1, NULL);
			LineTo(hDC, rect.right,  rect1.top - 1);
			MoveToEx(hDC, 0,  rect1.top - 3, NULL);
			LineTo(hDC, rect.right,  rect1.top - 3);
			EndPaint(hwnd, &ps);
			return 0;
	}
	return 0;
}

static void SetMainWndTitle()
{
    TCHAR *buf, *p;

    buf = malloc(4096);
    p = _mytcscpy(_mytcscpy(buf, T("TWC Design - [")), cur_project->path);
    *p++ = ']';
    *p = '\0';

    SetWindowText(hMainWnd, buf);
    free(buf);

    return;
}

static void SaveProjectToSelectedFile(TWCD_PROJECT *project)
{
    TCHAR *buf;
    int ret;

    buf = malloc(4096 * sizeof(TCHAR));
    buf[0] = '\0';
    if ( project->path != NULL ) {
        _tcscpy( buf, project->path);
    }
    ret = MyGetSaveFileName( hMainWnd, buf, 4096, T("twcp"), T("TWC Project (*.twcp)\0*.twcp"), T("Save project to:"));
    if ( ret <= 0 ) {
        if ( ret == -1 ) {
            MessageBox( hMainWnd, T("Error opening file!"), T("Error"), 0);
        }
        free( buf);
        return;
    }

    if (SaveProjectToFile( project, buf) == 0) {
        MessageBox( hMainWnd, T("Error writing file!"), T("Error"), 0);
    }
    SetString( &project->path, buf, 1);
    free( buf);
    SetStatusText( cur_project->path);
    SetMainWndTitle();

    return;
}

static int AskForSave()
{
    if (cur_project->path != NULL) {
        switch (MessageBox(hMainWnd, T("Save current project?"), T("TWC Design"), MB_YESNOCANCEL)) {
            case IDCANCEL: return 0;
            case IDYES: SaveProjectToFile(cur_project, NULL);
        }
    } else if ( GetProjectChildList( cur_project)->first != NULL ) {
        switch (MessageBox(hMainWnd, T("Save current project?"), T("TWC Design"), MB_YESNOCANCEL)) {
            case IDCANCEL: return 0;
            case IDYES: SaveProjectToSelectedFile(cur_project);
        }
    }
    return 1;
}

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//DLIST_NODE_PRT_OBJECT *node;
	//int i;

	switch (msg) {
		case WM_NCDESTROY:
			PostQuitMessage(0);
			break;
		case WM_CLOSE:
			if (AskForSave() == 0) return 0;
			UnloadCurrentProject();
			break;
		case WM_COMMAND:
			if (lParam == 0) {
				//Menu items
				switch (LOWORD(wParam)) {
					case ID_FILE_EXIT:
						if (AskForSave() == 0) return 0;
						PostQuitMessage(0);
						break;
					case ID_FILE_GENERATECODE:
						actGenerateCode();
						break;
					case ID_FILE_NEWPROJECT:
						actNewProject();
						break;
					case ID_FILE_NEWWINDOW:
						AddWindow();
						break;
					case ID_FILE_OPENPROJECT:
						actOpenProject();
						break;
					case ID_FILE_SAVEPROJECT:
						actSaveProject();
						break;
					case ID_FILE_SAVEPROJECTAS:
						SaveProjectToSelectedFile(cur_project);
						break;
                    case ID_FILE_SETTINGS:

                        break;
					case ID_ABOUT:
						DialogBoxParam(0, MAKEINTRESOURCE(IDD_DIALOG_ABOUT), hMainWnd, AboutDlgProc, 0);
						break;
				}
			} else {
                //Toolbox
				if ( (HWND)lParam == Toolbox.hwnd ) {
                    ProcessToolboxElemSelection( LOWORD(wParam));
                //Proplist
				} else if ( (HWND)lParam == hPropList ) {
					switch (HIWORD(wParam)) {
						case PLN_ITEMCHANGE:
							return OnChangeListProperty( hPropList, current_object, LOWORD(wParam));
					}
                //Main toolbar
				} else if ( (HWND)lParam == MainToolbar.hwnd ) {
					MainToolbarProc( LOWORD(wParam));
				}
			}
			return 0;
		case WM_SIZE:
			Resize(LOWORD(lParam), HIWORD(lParam));
			return 0;
		//case WM_ERASEBKGND:
			//return 1;
  }

  return DefFrameProc(hwnd, MDIClient.hwnd, msg, wParam, lParam);
}

void Resize(int width, int height)
{
    RECT rect;
    RECT status_rect;
    int tb_x, pl_y, tb_cx;

    GetClientRect(hMainWnd, &mainwnd_rect);
    GetClientRect(StatusBar.hwnd, &status_rect);
    SendMessage(StatusBar.hwnd, WM_SIZE,  0, 0);

    tb_x = MAX(mainwnd_rect.right * 4 / 5, mainwnd_rect.right - MAX_TOOLBAR_SIZE);
    pl_y = mainwnd_rect.bottom * 3 / 5;
    tb_cx = MIN(mainwnd_rect.right  / 5 + 1, MAX_TOOLBAR_SIZE);

    SendMessage(MainToolbar.hwnd, TB_GETITEMRECT,  0, (LPARAM)&rect);
    SendMessage(MainToolbar.hwnd, TB_SETBUTTONSIZE,  0, MAKELONG(MAINTOOLBAR_HEIGHT - rect.top - 2, MAINTOOLBAR_HEIGHT - rect.top - 2));
    SetWindowPos(MainToolbar.hwnd, 0, -1, -1, mainwnd_rect.right + 2, MAINTOOLBAR_HEIGHT + 1, SWP_NOZORDER);

    SetWindowPos(MDIClient.hwnd, 0, 0, MAINTOOLBAR_HEIGHT, tb_x, mainwnd_rect.bottom - status_rect.bottom - MAINTOOLBAR_HEIGHT, SWP_NOZORDER);

    SendMessage(Toolbox.hwnd, TB_SETBUTTONSIZE,  0, MAKELONG(tb_cx, 15));
    SetWindowPos(Toolbox.hwnd, NULL,  tb_x, MAINTOOLBAR_HEIGHT, tb_cx, pl_y - MAINTOOLBAR_HEIGHT, SWP_NOZORDER);
    SetWindowPos(hPropList, NULL, tb_x, pl_y, tb_cx, mainwnd_rect.bottom * 2 / 5 - status_rect.bottom + 1, SWP_NOZORDER);
    return;
}

void actGenerateCode()
{
    SetStatusText( T("Generating code..."));
    if ( GenerateProjectCode( cur_project) == 0 ) {
        MessageBox( hMainWnd, T("Error generating code!"), T("Error"), 0);
    }
    if ( cur_project->path ) {
        SetStatusText( cur_project->path);
    } else {
        SetStatusText( T("Ready!"));
    }
    return;
}

void actNewProject()
{
    if ( cur_project != NULL ) {
        if ( AskForSave() == 0 ) {
            return;
        }
        UnloadCurrentProject();
    }
    cur_project = NewProject();
    AddWindow();
    SetStatusText( T("<New project>"));
    return;
}

void actOpenProject()
{
    TCHAR *buf;
    int ret;

    if ( AskForSave() == 0 ) {
        return;
    }

    buf = malloc(4096 * sizeof(TCHAR));
    buf[0] = '\0';
    ret = MyGetOpenFileName( hMainWnd, buf, 4096, T("TWC Project (*.twcp)\0*.twcp\0All files(*.*)\0*.*"), T("Select project file"));
    if (ret <= 0) {
        if (ret == -1) {
            MessageBox( hMainWnd, T("Error opening file!"), T("Error"), 0);
        }
        free( buf);
        return;
    }

    UnloadCurrentProject();
    SetStatusText( T("Reading project from file..."));
    if ( LoadProjectFromFile( buf, &cur_project, &ret) == 0 ) {
        MessageBox( hMainWnd, T("Error parsing file!"), T("Error"), 0);
        SetStatusText( T("Error!"));
        free( buf);
        return;
    }

    SetStatusText( T("Loading project..."));
    LoadCurrentProject();
    SetStatusText( cur_project->path);
    SetMainWndTitle();
    free( buf);

    return;
}

void actSaveProject()
{
    if ( cur_project->path != NULL ) {
        SaveProjectToFile( cur_project, NULL);
    } else {
        SaveProjectToSelectedFile( cur_project);
    }
    return;
}

void actPreview()
{
    RT_OBJECT *obj;

    if ( !current_object ) {
        return;
    }
    obj = current_object;
    while ( obj->parent ) obj = obj->parent;
    PreviewWindow( obj, hMainWnd);
    return;
}