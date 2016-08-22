#pragma once
#include <Windows.h>
#include "Visuals.h"

HWND CreateDialogBox(HWND hwnd, HINSTANCE hInstance);
void RegisterDialogClass(HWND hwnd, HINSTANCE hInstance);
LRESULT CALLBACK DialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK DialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HWND hwndEditDiskName, hwndEditDiskSize;
	switch (msg) {

	case WM_CREATE:

		hwndEditDiskName = CreateWindowW(L"Edit", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			55, 30, 150, 20, hwnd, (HMENU)1,
			NULL, NULL);

		CreateWindowW(L"static", L"Name:",
			WS_CHILD | WS_VISIBLE, 20, 32, 35, 25, hwnd,
			(HMENU)6, NULL, NULL);

		hwndEditDiskSize = CreateWindowW(L"Edit", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			55, 55, 150, 20, hwnd, (HMENU)1,
			NULL, NULL);

		CreateWindowW(L"static", L"Size:",
			WS_CHILD | WS_VISIBLE, 20, 57, 35, 25, hwnd,
			(HMENU)6, NULL, NULL);

		EnumChildWindows(hwnd, (WNDENUMPROC)SetFont, (LPARAM)GetStockObject(DEFAULT_GUI_FONT));

		break;

	case WM_COMMAND:
		break;

	case WM_CLOSE:
		break;

	}

	return (DefWindowProcW(hwnd, msg, wParam, lParam));
}

void RegisterDialogClass(HWND hwnd, HINSTANCE hInstance) {

	WNDCLASSEXW wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.lpfnWndProc = (WNDPROC)DialogProc;
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpszClassName = L"DialogClass";
	RegisterClassExW(&wc);

}

HWND CreateDialogBox(HWND hwndParent, HINSTANCE hInstance) {

	//EnableWindow(hwndParent, FALSE); 
	return CreateWindow(L"DialogClass", L"Dialog Box",
		WS_VISIBLE | WS_SYSMENU | WS_CAPTION, 100, 100, 300, 200,
		hwndParent, NULL, hInstance, NULL);
}