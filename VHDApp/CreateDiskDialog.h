#pragma once
#include <Windows.h>
#include "Visuals.h"
#include "Conversions.h"

#define ID_BUTTON_CREATE_AND_MOUNT 4
#define ID_BUTTON_CREATE 14

HWND CreateDialogBox(HWND hwnd, HINSTANCE hInstance, LPCWSTR param, LPCWSTR title, int x = 100, int y = 100, int width = 300, int height = 200);
void RegisterDialogClass(HWND hwnd, HINSTANCE hInstance);
LRESULT CALLBACK DialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK DialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HWND hwndButtonCreateAndMount, hwndButtonCreate;
	static LPCWSTR userdata;
	static HWND hwndEditDiskSize, hwndEditDiskName, hwndEditFolderPath;

	switch (msg) {

	case WM_CREATE:

		hwndEditDiskName = CreateWindowW(L"Edit", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			45, 30, 220, 20, hwnd, (HMENU)1,
			NULL, NULL);

		CreateWindowW(L"static", L"Name:",
			WS_CHILD | WS_VISIBLE, 10, 32, 35, 25, hwnd,
			(HMENU)6, NULL, NULL);

		hwndEditDiskSize = CreateWindowW(L"Edit", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			45, 55, 220, 20, hwnd, (HMENU)1,
			NULL, NULL);

		CreateWindowW(L"static", L"Size:",
			WS_CHILD | WS_VISIBLE, 10, 57, 35, 25, hwnd,
			(HMENU)6, NULL, NULL);

		hwndEditFolderPath = CreateWindowW(L"Edit", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_AUTOHSCROLL,
			45, 80, 220, 20, hwnd, (HMENU)1,
			NULL, NULL);

		CreateWindowW(L"static", L"Folder:",
			WS_CHILD | WS_VISIBLE, 10, 82, 35, 25, hwnd,
			(HMENU)6, NULL, NULL);

		hwndButtonCreate = CreateWindowW(L"button", L"Create",
			WS_VISIBLE | WS_CHILD, 10, 115, 130, 25,
			hwnd, (HMENU)ID_BUTTON_CREATE, NULL, NULL);

		hwndButtonCreateAndMount = CreateWindowW(L"button", L"Create and mount",
			WS_VISIBLE | WS_CHILD, 145, 115, 130, 25,
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
			string strDiskName = WindowTextToString(hwndEditDiskName);

			string strFolderName = WindowTextToString(hwndEditFolderPath);

			string strDiskSize = WindowTextToString(hwndEditDiskSize);
			int size = stoi(strDiskSize);

			string strFullDiskPath = strFolderName + "\\" + strDiskSize;
			wstring wstrFullDiskPath = toWString(strFullDiskPath);

			//commented out for safety :p
			/*CreateVHD_Fixed(&wstrFullDiskPath[0], size);
			OpenAndAttachVHD2(&wstrFullDiskPath[0], CountPhysicalDisks());*/
		}

		if (LOWORD(wParam) == ID_BUTTON_CREATE)
		{
			string strDiskName = WindowTextToString(hwndEditDiskName);

			string strFolderName = WindowTextToString(hwndEditFolderPath);

			string strDiskSize = WindowTextToString(hwndEditDiskSize);
			int size = stoi(strDiskSize);

			string strFullDiskPath = strFolderName + "\\" + strDiskSize;
			wstring wstrFullDiskPath = toWString(strFullDiskPath);

			//commented out for safety :p
			//CreateVHD_Fixed(&wstrFullDiskPath[0], size);
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

HWND CreateDialogBox(HWND hwndParent, HINSTANCE hInstance, LPCWSTR param, LPCWSTR title, int x, int y, int width, int height) {

	//EnableWindow(hwndParent, FALSE); 
	return CreateWindowExW(WS_EX_DLGMODALFRAME | WS_EX_TOPMOST, L"DialogClass", title,
		WS_VISIBLE | WS_SYSMENU | WS_CAPTION, x, y, width, height,
		NULL, NULL, hInstance, (LPVOID)param);
}