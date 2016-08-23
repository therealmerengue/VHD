#pragma once
#include <Windows.h>
#include "Visuals.h"

#define ID_BUTTON_CREATE_AND_MOUNT 4

HWND CreateDialogBox(HWND hwnd, HINSTANCE hInstance, LPCWSTR param);
void RegisterDialogClass(HWND hwnd, HINSTANCE hInstance);
LRESULT CALLBACK DialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK DialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HWND hwndEditDiskName, hwndButtonCreateAndMount, hwndEditFolderPath;
	static LPCWSTR userdata;
	static HWND hwndEditDiskSize;

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

		hwndEditFolderPath = CreateWindowW(L"Edit", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY,
			55, 80, 150, 20, hwnd, (HMENU)1,
			NULL, NULL);

		CreateWindowW(L"static", L"Folder:",
			WS_CHILD | WS_VISIBLE, 20, 82, 35, 25, hwnd,
			(HMENU)6, NULL, NULL);

		hwndButtonCreateAndMount = CreateWindowW(L"button", L"Create and mount",
			WS_VISIBLE | WS_CHILD, 115, 115, 90, 25,
			hwnd, (HMENU)ID_BUTTON_CREATE_AND_MOUNT, NULL, NULL);

		static CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		userdata = reinterpret_cast<LPCWSTR>(pCreate->lpCreateParams);

		SetWindowText(hwndEditFolderPath, userdata);

		EnumChildWindows(hwnd, (WNDENUMPROC)SetFont, (LPARAM)GetStockObject(DEFAULT_GUI_FONT));

		break;

	case WM_COMMAND:
	{
		if (LOWORD(wParam) == ID_BUTTON_CREATE_AND_MOUNT) 
		{
			wchar_t diskNameBuffer[128];
			GetWindowText(hwndEditDiskName, diskNameBuffer, 128);
			wstring wstrDiskName = wstring(diskNameBuffer);
			string strDiskSize = toString(wstrDiskName);

			wstring wstrFolderName = wstring(userdata);
			string strFolderName = toString(wstrFolderName);

			wchar_t diskSizeBuffer[128]; // TODO: move to method window text to string
			GetWindowText(hwndEditDiskSize, diskSizeBuffer, 128);
			wstring wstrDiskSize = wstring(diskSizeBuffer);
			string strDiskSize = toString(wstrDiskSize);
			int size = stoi(strDiskSize);

			string strFullDiskPath = strFolderName + "\\" + strDiskSize;
			wstring wstrFullDiskPath = toWString(strFullDiskPath);

			CreateVHD_Fixed(&wstrFullDiskPath[0], size);
		}

		break;
	}
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

HWND CreateDialogBox(HWND hwndParent, HINSTANCE hInstance, LPCWSTR param) {

	//EnableWindow(hwndParent, FALSE); 
	return CreateWindowExW(WS_EX_DLGMODALFRAME | WS_EX_TOPMOST, L"DialogClass", L"Dialog Box",
		WS_VISIBLE | WS_SYSMENU | WS_CAPTION, 100, 100, 300, 200,
		NULL, NULL, hInstance, (LPVOID)param);
}