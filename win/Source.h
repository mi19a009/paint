/*
Copyright 2025 Taichi Murakami.
*/

#pragma once
#include <windows.h>
#define MAXCUSTCOLORS           16

/* Window Classes */
#define APPLICATIONCLASSNAME    TEXT("Paint")
#define CANVASCLASSNAME         TEXT("Canvas")
#define DOCUMENTCLASSNAME       TEXT("Document")
#define MDICLIENTCLASSNAME      TEXT("MDIClient")

/* Window Extra */
#ifdef _WIN64
#define APPLICATIONWINDOWEXTRA  16
#else
#define APPLICATIONWINDOWEXTRA  12
#endif

enum APPLICATION_WINDOW_MESSAGE
{
	WM_ABOUT = WM_APP,
	WM_CLOSEDOCUMENT,
	WM_CLOSEDOCUMENTALL,
	WM_NEW,
	WM_TRANSLATEMESSAGE,
};

typedef struct tagABOUTDIALOG
{
	HWND hwndOwner;
	HINSTANCE hInstance;
} ABOUTDIALOG, FAR *LPABOUTDIALOG;

EXTERN_C BOOL WINAPI AboutDialog(_In_ LPABOUTDIALOG lpParam);
EXTERN_C int WINAPI ErrorMessageBox(_In_opt_ HWND hWnd, _In_opt_ HINSTANCE hInstance, _In_ DWORD dwError);
EXTERN_C BOOL WINAPI Help(_In_opt_ HWND hWnd);
EXTERN_C BOOL WINAPI MoveWindowForRect(_In_ HWND hWnd, _In_ CONST RECT FAR *lpRect, _In_ BOOL bRepaint);
EXTERN_C BOOL WINAPI MoveWindowToCenter(_In_ HWND hWnd, _In_ BOOL bRepaint);
