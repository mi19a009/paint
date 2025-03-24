/*
Copyright 2025 Taichi Murakami.
アプリケーションのメイン エントリ ポイントを実装します。
特定のウィンドウに属しない関数を実装します。
*/

#include <Windows.h>
#include <CommCtrl.h>
#include <PathCch.h>
#include <tchar.h>
#include "Source.h"
#include "Resource.h"
#define ERRORMESSAGEBOXFLAGS (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM)
#define CCH_CAPTION 32

EXTERN_C LRESULT CALLBACK ApplicationWindowProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
static HWND WINAPI CreateApplicationWindow(_In_opt_ HINSTANCE hInstance);
static BOOL WINAPI InitializeClasses(_In_opt_ HINSTANCE hInstance);
static BOOL WINAPI InitializeComponents(void);
static BOOL WINAPI InitializeControls(void);

#define Initialize(hInstance) (InitializeComponents() && InitializeControls() && InitializeClasses(hInstance))

EXTERN_C int APIENTRY _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInst, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
	HACCEL hAccel;
	HWND hWnd;
	MSG msg;
	int nResult = -1;

	if (Initialize(hInstance))
	{
		hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_MAIN));

		if (hAccel)
		{
			hWnd = CreateApplicationWindow(hInstance);

			if (hWnd)
			{
				ShowWindow(hWnd, nCmdShow);
				UpdateWindow(hWnd);

				while ((nResult = GetMessage(&msg, NULL, 0, 0)) > 0)
				{
					if (!TranslateAccelerator(hWnd, hAccel, &msg) && !SendMessage(hWnd, WM_TRANSLATEMESSAGE, 0, (LPARAM)&msg))
					{
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
				}
				if (!nResult)
				{
					nResult = (int)msg.wParam;
				}
			}
		}
	}
	if (nResult)
	{
		ErrorMessageBox(NULL, hInstance, GetLastError());
	}

	return nResult;
}

EXTERN_C int WINAPI ErrorMessageBox(_In_opt_ HWND hWnd, _In_opt_ HINSTANCE hInstance, _In_ DWORD dwError)
{
	LPCTSTR lpCaption;
	LPTSTR lpText = NULL;
	int nResult;
	TCHAR strCaption[CCH_CAPTION];

	if (!hInstance && hWnd)
	{
		hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
	}
	if (LoadString(hInstance, IDS_MAIN, strCaption, ARRAYSIZE(strCaption)))
	{
		lpCaption = strCaption;
	}
	else
	{
		lpCaption = NULL;
	}
	if (FormatMessage(ERRORMESSAGEBOXFLAGS, NULL, dwError, 0, (LPWSTR)&lpText, 0, NULL))
	{
		nResult = MessageBox(hWnd, lpText, lpCaption, MB_ICONERROR);
	}
	else
	{
		nResult = 0;
	}

	LocalFree(lpText);
	return nResult;
}

EXTERN_C BOOL WINAPI Help(_In_opt_ HWND hWnd)
{
	LPTSTR lpFileName;
	HRESULT hResult = S_OK;
	BOOL bResult = FALSE;
	lpFileName = LocalAlloc(LPTR, PATHCCH_MAX_CCH * sizeof(TCHAR));

	if (lpFileName)
	{
		if (GetModuleFileName(NULL, lpFileName, PATHCCH_MAX_CCH))
		{
			hResult = PathCchRemoveFileSpec(lpFileName, PATHCCH_MAX_CCH);

			if (SUCCEEDED(hResult))
			{
				hResult = PathCchAppend(lpFileName, PATHCCH_MAX_CCH, TEXT("Help\\index.html"));

				if (SUCCEEDED(hResult))
				{
					bResult = (INT_PTR)ShellExecute(hWnd, NULL, lpFileName, NULL, NULL, SW_NORMAL) > 32;
				}
			}
		}

		LocalFree(lpFileName);
	}
	if (FAILED(hResult))
	{
		SetLastError(hResult);
	}

	return bResult;
}

EXTERN_C BOOL WINAPI MoveWindowForRect(_In_ HWND hWnd, _In_ CONST RECT FAR *lpRect, _In_ BOOL bRepaint)
{
	const LONG X = lpRect->left;
	const LONG Y = lpRect->top;
	const LONG nWidth = lpRect->right - X;
	const LONG nHeight = lpRect->bottom - Y;
	return MoveWindow(hWnd, X, Y, nWidth, nHeight, bRepaint);
}

EXTERN_C BOOL WINAPI MoveWindowToCenter(_In_ HWND hWnd, _In_ BOOL bRepaint)
{
	HWND hWndParent;
	RECT rcWnd, rcWndParent;
	BOOL bResult;
	hWndParent = (HWND)GetWindowLongPtr(hWnd, GWLP_HWNDPARENT);

	if (hWndParent && GetWindowRect(hWnd, &rcWnd) && GetWindowRect(hWndParent, &rcWndParent))
	{
		rcWnd.right -= rcWnd.left;
		rcWnd.bottom -= rcWnd.top;
		rcWndParent.right -= rcWndParent.left;
		rcWndParent.bottom -= rcWndParent.top;
		rcWnd.left = rcWndParent.left + (rcWndParent.right - rcWnd.right) / 2;
		rcWnd.top = rcWndParent.top + (rcWndParent.bottom - rcWnd.bottom) / 2;
		bResult = SetWindowPos(hWnd, NULL, rcWnd.left, rcWnd.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}

static HWND WINAPI CreateApplicationWindow(_In_opt_ HINSTANCE hInstance)
{
	LPCTSTR lpCaption;
	TCHAR strCaption[CCH_CAPTION];

	if (LoadString(hInstance, IDS_MAIN, strCaption, ARRAYSIZE(strCaption)))
	{
		lpCaption = strCaption;
	}
	else
	{
		lpCaption = APPLICATIONCLASSNAME;
	}

	return CreateWindow(APPLICATIONCLASSNAME, lpCaption, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
}

static BOOL WINAPI InitializeClasses(_In_opt_ HINSTANCE hInstance)
{
	WNDCLASSEX wc;
	ATOM bResult;
	ZeroMemory(&wc, sizeof wc);

	/* Application */
	wc.cbSize = sizeof wc;
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = ApplicationWindowProc;
	wc.cbWndExtra = APPLICATIONWINDOWEXTRA;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAIN));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MAIN);
	wc.lpszClassName = APPLICATIONCLASSNAME;
	bResult = RegisterClassEx(&wc);

	return bResult;
}

static BOOL WINAPI InitializeComponents(void)
{
	HRESULT hResult;
	BOOL bResult;
	hResult = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	if (SUCCEEDED(hResult))
	{
		hResult = S_OK;
		bResult = TRUE;
	}
	else
	{
		bResult = FALSE;
	}

	SetLastError(hResult);
	return bResult;
}

static BOOL WINAPI InitializeControls(void)
{
	INITCOMMONCONTROLSEX icc;
	ZeroMemory(&icc, sizeof icc);
	icc.dwSize = sizeof icc;
	icc.dwICC = ICC_BAR_CLASSES;
	return InitCommonControlsEx(&icc);
}
