#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include "dirent.h"
#include "VHD.h"
#include "Files.h"
#include "resource.h"

HINSTANCE g_hinst;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
UINT_PTR CALLBACK NewDiscDialogProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);

void PrintLastError(HWND hwnd);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PWSTR pCmdLine, int nCmdShow) {

	MSG  msg;
	HWND hwnd;
	WNDCLASSW wc;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.lpszClassName = L"Window";
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpszMenuName = NULL;
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassW(&wc);
	hwnd = CreateWindowW(wc.lpszClassName, L"Window",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		100, 100, 350, 250, NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0)) {

		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg,
	WPARAM wParam, LPARAM lParam) {

	static HWND hwndButton;

	switch (msg) {

	case WM_CREATE:
		hwndButton = CreateWindowW(L"button", L"Create",
			WS_VISIBLE | WS_CHILD, 10, 10, 130, 25,
			hwnd, (HMENU)1, NULL, NULL);
		break;

	case WM_COMMAND:
		if (LOWORD(wParam) == 1)
		{
			auto ip = DialogBoxParam(g_hinst, MAKEINTRESOURCE(IDD_ABOUT), hwnd, (DLGPROC)NewDiscDialogProc, (LPARAM)L"path");
		}
		break;

	case WM_DESTROY:

		PostQuitMessage(0);
		break;
	}

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}

UINT_PTR CALLBACK NewDiscDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	static LPCWSTR path;
	switch (uMsg) 
	{
	case WM_INITDIALOG:
		path = (LPCWSTR)lParam;
		break;
	case WM_COMMAND:
		if (LOWORD(wParam == IDCANCEL))
			EndDialog(hwndDlg, IDCANCEL);
		if (LOWORD(wParam == IDOK))
			MessageBox(hwndDlg, path, L"Path", MB_OK);
		break;

	case WM_QUIT:
		EndDialog(hwndDlg, 0);
		break;
	case WM_DESTROY:
		EndDialog(hwndDlg, 0);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

void PrintLastError(HWND hwnd)
{
	DWORD err = GetLastError();
	wchar_t szTest[10];
	swprintf_s(szTest, 10, L"%d", err);
	MessageBox(hwnd, szTest, L"error", MB_OK);
}