/*
Copyright 2025 Taichi Murakami.
バージョン情報ダイアログ プロシージャを実装します。
*/

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "Source.h"
#include "Resource.h"

static INT_PTR CALLBACK DialogProc(_In_ HWND hDlg, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
static INT_PTR WINAPI OnClose(_In_ HWND hDlg, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
static INT_PTR WINAPI OnCommand(_In_ HWND hDlg, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
static INT_PTR WINAPI OnInitDialog(_In_ HWND hDlg, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

EXTERN_C BOOL WINAPI AboutDialog(_In_ LPABOUTDIALOG lpParam)
{
	return DialogBoxParam(lpParam->hInstance, MAKEINTRESOURCE(IDD_ABOUT), lpParam->hwndOwner, DialogProc, (LPARAM)lpParam) == IDOK;
}

static INT_PTR CALLBACK DialogProc(_In_ HWND hDlg, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	INT_PTR bResult;

	switch (uMsg)
	{
	case WM_CLOSE:
		bResult = OnClose(hDlg, uMsg, wParam, lParam);
		break;
	case WM_COMMAND:
		bResult = OnCommand(hDlg, uMsg, wParam, lParam);
		break;
	case WM_INITDIALOG:
		bResult = OnInitDialog(hDlg, uMsg, wParam, lParam);
		break;
	default:
		bResult = FALSE;
		break;
	}

	return bResult;
}

static INT_PTR WINAPI OnClose(_In_ HWND hDlg, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	EndDialog(hDlg, 0);
	return FALSE;
}

static INT_PTR WINAPI OnCommand(_In_ HWND hDlg, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	EndDialog(hDlg, LOWORD(wParam));
	return FALSE;
}

static INT_PTR WINAPI OnInitDialog(_In_ HWND hDlg, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	MoveWindowToCenter(hDlg, FALSE);
	return TRUE;
}
