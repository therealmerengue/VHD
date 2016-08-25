#pragma once
#include <Windows.h>
#include "Visuals.h"
#include "StringOperations.h"
#include "Combobox.h"

#define ID_BUTTON_CREATE_AND_MOUNT 4
#define ID_BUTTON_CHOOSE_DISK 8
#define ID_BUTTON_CHOOSE_FOLDER_TO_SORT 9
#define ID_BUTTON_SORT_FOLDER 10
#define ID_BUTTON_CREATE 14
#define ID_COMBOBOX 16
#define ID_BUTTON_CANCEL 17
#define ID_BUTTON_CREATE_FOLDERS_FROM_DIALOG 18
#define ID_CHECKBOX_SORT_FOLDER 30
#define ID_CHECKBOX_ENCRYPT_FOLDER 31

string g_diskPath;
HINSTANCE g_hinst;

HWND CreateDialogBox(HWND hwndParent, HINSTANCE hInstance, LPCWSTR param, LPCWSTR lpClassName, LPCWSTR title, int x = 100, int y = 100, int width = 300, int height = 200);
void RegisterDialogClass(HWND hwnd, HINSTANCE hInstance, LPCWSTR lpszClassName, WNDPROC lpfnWndProc);
void OpenFileDialog(HWND hwnd);
PIDLIST_ABSOLUTE OpenFolderDialog(HWND hwnd);

void RegisterDialogClass(HWND hwnd, HINSTANCE hInstance, LPCWSTR lpszClassName, WNDPROC lpfnWndProc) 
{
	WNDCLASSEXW wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.lpfnWndProc = lpfnWndProc;
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpszClassName = lpszClassName;
	RegisterClassExW(&wc);
}

HWND CreateDialogBox(HWND hwndParent, HINSTANCE hInstance, LPCWSTR param, LPCWSTR lpClassName, LPCWSTR title, int x, int y, int width, int height) 
{
	return CreateWindowExW(WS_EX_DLGMODALFRAME | WS_EX_TOPMOST, lpClassName, title,
		WS_VISIBLE | WS_SYSMENU | WS_CAPTION, x, y, width, height,
		NULL, NULL, hInstance, (LPVOID)param);
}

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

void OpenFileDialog(HWND hwnd)
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

		OpenAndAttachVHD2(ofn.lpstrFile, CountPhysicalDisks());
	}
}