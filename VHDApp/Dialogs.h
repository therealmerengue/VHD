#pragma once
#include <Windows.h>
#include "Visuals.h"
#include "StringOperations.h"
#include "Combobox.h"

//void RegisterDialogClass(HWND hwnd, HINSTANCE hInstance, LPCWSTR lpszClassName, WNDPROC lpfnWndProc) 
//{
//	WNDCLASSEXW wc = { 0 };
//	wc.cbSize = sizeof(WNDCLASSEXW);
//	wc.lpfnWndProc = lpfnWndProc;
//	wc.hInstance = hInstance;
//	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
//	wc.lpszClassName = lpszClassName;
//	RegisterClassExW(&wc);
//}
//
//HWND CreateDialogBox(HWND hwndParent, HINSTANCE hInstance, LPCWSTR param, LPCWSTR lpClassName, LPCWSTR title, int x, int y, int width, int height) 
//{
//	return CreateWindowExW(WS_EX_DLGMODALFRAME | WS_EX_TOPMOST, lpClassName, title,
//		WS_VISIBLE | WS_SYSMENU | WS_CAPTION, x, y, width, height,
//		NULL, NULL, hInstance, (LPVOID)param);
//}

PIDLIST_ABSOLUTE OpenFolderDialog(HWND hwnd)
{
	BROWSEINFO bi;
	int iImage = 0;
	bi.hwndOwner = hwnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = NULL;
	bi.lpszTitle = TEXT("Choose folder");
	bi.ulFlags = BIF_NEWDIALOGSTYLE;
	bi.lpfn = NULL;
	bi.lParam = NULL;
	bi.iImage = iImage;

	return SHBrowseForFolder(&bi);
}

void OpenFileDialog(HWND hwnd, Action action = ATTACH_DISK)
{
	OPENFILENAME ofn;
	TCHAR szFile[MAX_PATH];

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.hwndOwner = hwnd;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = TEXT("Only VHDs(*.vhd)\0*.VHD\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrInitialDir = L"C:\\";
	ofn.lpstrFileTitle = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn))
	{
		SetWindowText(hwnd, ofn.lpstrFile);

		if (action == ATTACH_DISK)
			OpenAndAttachVHD2(ofn.lpstrFile, CountPhysicalDisks());
		if (action == DETACH_DISK)
			OpenAndDetachVHD(ofn.lpstrFile);
	}
}

void ShowTimeDialog(HWND hwnd, std::chrono::steady_clock::time_point begin_time, std::chrono::steady_clock::time_point end_time, wstring wstrMessageBeginning)
{
	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time).count();
	wstring wstrTime = to_wstring(time);
	wstring wstrMessage = wstrMessageBeginning + wstrTime + L"ms";
	MessageBox(hwnd, &wstrMessage[0], L"Sorted", MB_OK);
}