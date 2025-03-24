/*
Copyright 2025 Taichi Murakami.
アプリケーション ウィンドウ プロシージャを実装します。
アプリケーション ウィンドウが閉じられた場合は現在のアプリケーションを終了します。
*/

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stddef.h>
#include "Source.h"
#include "Resource.h"
#define CCH_CAPTION             32
#define FLAG_QUIT               0x00000001
#define ID_FIRSTCHILD           0x8000
#define ID_HWNDCLIENT           1
#define ID_WINDOWMENU           1
#define MINTRACKSIZE_X          400
#define MINTRACKSIZE_Y          300
#define WS_HWNDCLIENT           (WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL)
#define WS_EX_HWNDCLIENT        WS_EX_CLIENTEDGE
#define MAKECOMMAND(f, m, w, l) (lpProc) = (f); (uMsg) = (m); (wParam) = (w); (lParam) = (l)

typedef struct tagAPPLICATIONWINDOWEXTRA
{
	LONG_PTR lpClientWindow;
	LONG dwDpi;
	LONG dwFlags;
} WINDOWEXTRA;

static_assert(sizeof(WINDOWEXTRA) == APPLICATIONWINDOWEXTRA, "APPLICATIONWINDOWEXTRA");
#define GWLP_HWNDCLIENT         offsetof(WINDOWEXTRA, lpClientWindow)
#define GWL_DPI                 offsetof(WINDOWEXTRA, dwDpi)
#define GWL_FLAGS               offsetof(WINDOWEXTRA, dwFlags)

static HWND WINAPI CreateClient(_In_ HWND hWnd, _In_ HINSTANCE hInstance);
static LRESULT WINAPI DefProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
static LRESULT WINAPI SendClientMessage(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
static LRESULT WINAPI SendDocumentMessage(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
static LRESULT WINAPI SendMessageHelp(_In_ HWND hWnd);
static LRESULT WINAPI OnAbout(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
static LRESULT WINAPI OnClose(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
static LRESULT WINAPI OnCloseDocument(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
static LRESULT WINAPI OnCloseDocumentAll(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
static LRESULT WINAPI OnCommand(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
static LRESULT WINAPI OnCreate(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
static LRESULT WINAPI OnDestroy(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
static LRESULT WINAPI OnDpiChanged(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
static LRESULT WINAPI OnGetMinMaxInfo(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
static LRESULT WINAPI OnHelp(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
static LRESULT WINAPI OnNew(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
static LRESULT WINAPI OnParentNotify(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
static LRESULT WINAPI OnTranslateMessage(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

EXTERN_C LRESULT CALLBACK ApplicationWindowProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	FARPROC lpProc;

	switch (uMsg)
	{
	case WM_ABOUT:
		lpProc = OnAbout;
		break;
	case WM_CLOSE:
		lpProc = OnClose;
		break;
	case WM_CLOSEDOCUMENT:
		lpProc = OnCloseDocument;
		break;
	case WM_CLOSEDOCUMENTALL:
		lpProc = OnCloseDocumentAll;
		break;
	case WM_COMMAND:
		lpProc = OnCommand;
		break;
	case WM_CREATE:
		lpProc = OnCreate;
		break;
	case WM_DESTROY:
		lpProc = OnDestroy;
		break;
	case WM_DPICHANGED:
		lpProc = OnDpiChanged;
		break;
	case WM_GETMINMAXINFO:
		lpProc = OnGetMinMaxInfo;
		break;
	case WM_HELP:
		lpProc = OnHelp;
		break;
	case WM_NEW:
		lpProc = OnNew;
		break;
	case WM_PARENTNOTIFY:
		lpProc = OnParentNotify;
		break;
	case WM_TRANSLATEMESSAGE:
		lpProc = OnTranslateMessage;
		break;
	default:
		lpProc = DefProc;
		break;
	}

	return lpProc(hWnd, uMsg, wParam, lParam);
}

static HWND WINAPI CreateClient(_In_ HWND hWnd, _In_ HINSTANCE hInstance)
{
	CLIENTCREATESTRUCT ccs;
	ZeroMemory(&ccs, sizeof ccs);
	ccs.hWindowMenu = GetMenu(hWnd);
	ccs.idFirstChild = ID_FIRSTCHILD;

	if (ccs.hWindowMenu)
	{
		ccs.hWindowMenu = GetSubMenu(ccs.hWindowMenu, ID_WINDOWMENU);
	}

	return CreateWindowEx(WS_EX_HWNDCLIENT, MDICLIENTCLASSNAME, NULL, WS_HWNDCLIENT, 0, 0, 0, 0, hWnd, (HMENU)ID_HWNDCLIENT, hInstance, &ccs);
}

static LRESULT WINAPI DefProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	return DefFrameProc(hWnd, (HWND)GetWindowLongPtr(hWnd, GWLP_HWNDCLIENT), uMsg, wParam, lParam);
}

static LRESULT WINAPI SendClientMessage(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	LRESULT nResult;
	hWnd = (HWND)GetWindowLongPtr(hWnd, GWLP_HWNDCLIENT);

	if (hWnd)
	{
		nResult = SendMessage(hWnd, uMsg, wParam, lParam);
	}
	else
	{
		nResult = 0;
	}

	return nResult;
}

static LRESULT WINAPI SendDocumentMessage(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	LRESULT nResult = 0;
	hWnd = (HWND)GetWindowLongPtr(hWnd, GWLP_HWNDCLIENT);

	if (hWnd)
	{
		hWnd = (HWND)SendMessage(hWnd, WM_MDIGETACTIVE, 0, 0);

		if (hWnd)
		{
			nResult = SendMessage(hWnd, uMsg, wParam, lParam);
		}
	}

	return nResult;
}

static LRESULT WINAPI SendMessageHelp(_In_ HWND hWnd)
{
	HELPINFO info;
	ZeroMemory(&info, sizeof info);
	return SendMessage(hWnd, WM_HELP, 0, (LPARAM)&info);
}

static LRESULT WINAPI OnAbout(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	ABOUTDIALOG param;
	ZeroMemory(&param, sizeof param);
	param.hwndOwner = hWnd;
	param.hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
	AboutDialog(&param);
	return 0;
}

static LRESULT WINAPI OnClose(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	LRESULT nResult;
	nResult = SendMessage(hWnd, WM_CLOSEDOCUMENTALL, 0, 0);

	if (nResult == 0)
	{
		nResult = DefProc(hWnd, uMsg, wParam, lParam);
	}

	return nResult;
}

static LRESULT WINAPI OnCloseDocument(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	return SendDocumentMessage(hWnd, WM_CLOSE, 0, 0);
}

static LRESULT WINAPI OnCloseDocumentAll(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	HWND hWndNext;
	LRESULT nResult = 0;
	hWnd = (HWND)GetWindowLongPtr(hWnd, GWLP_HWNDCLIENT);

	if (hWnd)
	{
		hWnd = GetWindow(hWnd, GW_CHILD);

		while (hWnd)
		{
			hWndNext = GetWindow(hWnd, GW_HWNDNEXT);
			nResult = SendMessage(hWnd, WM_CLOSE, 0, 0);

			if (nResult)
			{
				hWnd = NULL;
			}
			else
			{
				hWnd = hWndNext;
			}
		}
	}

	return nResult;
}

static LRESULT WINAPI OnCommand(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	LRESULT nResult;

	switch (LOWORD(wParam))
	{
	case IDM_ABOUT:
		nResult = SendMessage(hWnd, WM_ABOUT, 0, 0);
		break;
	case IDM_CLOSE:
		nResult = SendMessage(hWnd, WM_CLOSE, 0, 0);
		break;
	case IDM_HELP:
		nResult = SendMessageHelp(hWnd);
		break;
	case IDM_NEW:
		nResult = SendMessage(hWnd, WM_NEW, 0, 0);
		break;
	default:
		nResult = DefProc(hWnd, uMsg, wParam, lParam);
		break;
	}

	return nResult;
}

static LRESULT WINAPI OnCreate(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	HINSTANCE hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
	LRESULT nResult = -1;
	SetWindowLong(hWnd, GWL_DPI, GetDpiForWindow(hWnd));

	if (CreateClient(hWnd, hInstance) != NULL)
	{
		SetWindowLong(hWnd, GWL_FLAGS, FLAG_QUIT | GetWindowLong(hWnd, GWL_FLAGS));
		nResult = DefProc(hWnd, uMsg, wParam, lParam);
	}

	return nResult;
}

static LRESULT WINAPI OnDestroy(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	if (GetWindowLong(hWnd, GWL_FLAGS) & FLAG_QUIT)
	{
		PostQuitMessage(0);
	}

	return DefProc(hWnd, uMsg, wParam, lParam);
}

static LRESULT WINAPI OnDpiChanged(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	SetWindowLong(hWnd, GWL_DPI, LOWORD(wParam));
	MoveWindowForRect(hWnd, (LPRECT)lParam, TRUE);
	return DefProc(hWnd, uMsg, wParam, lParam);
}

static LRESULT WINAPI OnGetMinMaxInfo(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	RECT rc = { 0, 0, MINTRACKSIZE_X, MINTRACKSIZE_Y };
	LRESULT nResult;
	nResult = DefProc(hWnd, uMsg, wParam, lParam);
	AdjustWindowRectExForDpi(&rc, WS_OVERLAPPEDWINDOW, TRUE, 0, GetWindowLong(hWnd, GWL_DPI));
	((LPMINMAXINFO)lParam)->ptMinTrackSize.x = rc.right - rc.left;
	((LPMINMAXINFO)lParam)->ptMinTrackSize.y = rc.bottom - rc.top;
	return nResult;
}

static LRESULT WINAPI OnHelp(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	Help(hWnd);
	return TRUE;
}

static LRESULT WINAPI OnNew(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	return 0;
}

static LRESULT WINAPI OnParentNotify(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	LRESULT nResult;

	switch ((DWORD)wParam)
	{
	case MAKEWPARAM(WM_CREATE, ID_HWNDCLIENT):
		SetWindowLongPtr(hWnd, GWLP_HWNDCLIENT, lParam);
		nResult = 0;
		break;
	case MAKEWPARAM(WM_DESTROY, ID_HWNDCLIENT):
		SetWindowLongPtr(hWnd, GWLP_HWNDCLIENT, 0);
		nResult = 0;
		break;
	default:
		nResult = DefProc(hWnd, uMsg, wParam, lParam);
		break;
	}

	return nResult;
}

static LRESULT WINAPI OnTranslateMessage(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	HWND hWndChild;
	BOOL bResult;
	hWndChild = (HWND)GetWindowLongPtr(hWnd, GWLP_HWNDCLIENT);

	if (hWndChild)
	{
		bResult = TranslateMDISysAccel(hWndChild, (LPMSG)lParam);
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}
