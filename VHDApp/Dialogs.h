#pragma once
#include <Windows.h>
#include "Visuals.h"
#include "StringOperations.h"
#include "Combobox.h"

#define ID_BUTTON_CREATE_AND_MOUNT 4
#define ID_BUTTON_CREATE_FOLDERS 8
#define ID_BUTTON_CHOOSE_FOLDER_TO_SORT 9
#define ID_BUTTON_SORT_FOLDER 10
#define ID_BUTTON_CREATE 14
#define ID_COMBOBOX 16
#define ID_CHECKBOX_SORT_FOLDER 30
#define ID_CHECKBOX_ENCRYPT_FOLDER 31

string g_diskPath;
HINSTANCE g_hinst;

HWND CreateDialogBox(HWND hwndParent, HINSTANCE hInstance, LPCWSTR param, LPCWSTR lpClassName, LPCWSTR title, int x = 100, int y = 100, int width = 300, int height = 200);
void RegisterDialogClass(HWND hwnd, HINSTANCE hInstance, LPCWSTR lpszClassName, WNDPROC lpfnWndProc);
void OpenFileDialog(HWND hwnd);
PIDLIST_ABSOLUTE OpenFolderDialog(HWND hwnd);

LRESULT CALLBACK DiskDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK CreateFoldersDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ChooseFolderToSortDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK ChooseFolderToSortDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndEditFolderToSort, hwndButtonChooseFolderToSort, hwndButtonSortFolder;

	switch (msg) 
	{
	case WM_CREATE:
	{
		CreateWindowW(L"static", L"Folder:",
			WS_CHILD | WS_VISIBLE, 10, 32, 35, 25, hwnd,
			(HMENU)6, NULL, NULL);

		hwndEditFolderToSort = CreateWindowW(L"Edit", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_AUTOHSCROLL,
			55, 30, 150, 20, hwnd, (HMENU)1,
			NULL, NULL);

		hwndButtonChooseFolderToSort = CreateWindowW(L"button", L"Choose folder",
			WS_VISIBLE | WS_CHILD, 10, 105, 90, 25,
			hwnd, (HMENU)ID_BUTTON_CHOOSE_FOLDER_TO_SORT, NULL, NULL);

		hwndButtonSortFolder = CreateWindowW(L"button", L"Sort",
			WS_VISIBLE | WS_CHILD, 105, 105, 90, 25,
			hwnd, (HMENU)ID_BUTTON_SORT_FOLDER, NULL, NULL);

		EnumChildWindows(hwnd, (WNDENUMPROC)SetFont, (LPARAM)GetStockObject(DEFAULT_GUI_FONT));

		break;
	}
	case WM_COMMAND:
	{
		if (LOWORD(wParam) == ID_BUTTON_CHOOSE_FOLDER_TO_SORT) 
		{
			wchar_t folderPath[MAX_PATH + 1];
			auto pidl = OpenFolderDialog(hwnd);
			if (pidl)
			{
				SHGetPathFromIDList(pidl, folderPath);
				SetWindowText(hwndEditFolderToSort, folderPath);
			}
		}

		if (LOWORD(wParam) == ID_BUTTON_SORT_FOLDER) 
		{
			//sort
		}

		break;
	}
	case WM_CLOSE:

		break;

	case WM_DESTROY:
		break;
	}

	return (DefWindowProcW(hwnd, msg, wParam, lParam));
}

LRESULT CALLBACK CreateFoldersDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndCombo, hwndButtonChooseDisk;

	switch (msg) 
	{
	case WM_CREATE:

		CreateWindowW(L"static", L"Disk:",
			WS_CHILD | WS_VISIBLE, 10, 14, 35, 25, hwnd,
			(HMENU)6, NULL, NULL);

		hwndCombo = CreateWindowW(L"Combobox", NULL,
			WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
			55, 12, 50, 65, hwnd, (HMENU)ID_COMBOBOX, NULL, NULL);

		AddItemsToCombobox(hwndCombo, GetDriveLetters());

		hwndButtonChooseDisk = CreateWindowW(L"button", L"Create folders",
			WS_VISIBLE | WS_CHILD, 95, 130, 90, 25,
			hwnd, (HMENU)ID_BUTTON_CREATE_FOLDERS, NULL, NULL);

		CreateWindowW(L"Button", L"Choose folders",
			WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
			5, 40, 270, 85, hwnd, (HMENU)0, NULL, NULL);

		CreateWindowW(L"button", L"Sort folder",
			WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
			10, 55, 100, 20, hwnd, (HMENU)ID_CHECKBOX_SORT_FOLDER,
			NULL, NULL);

		CreateWindowW(L"button", L"Encrypt folder",
			WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
			10, 75, 100, 20, hwnd, (HMENU)ID_CHECKBOX_ENCRYPT_FOLDER,
			NULL, NULL);

		CheckDlgButton(hwnd, ID_CHECKBOX_SORT_FOLDER, BST_UNCHECKED);
		CheckDlgButton(hwnd, ID_CHECKBOX_ENCRYPT_FOLDER, BST_UNCHECKED);

		EnumChildWindows(hwnd, (WNDENUMPROC)SetFont, (LPARAM)GetStockObject(DEFAULT_GUI_FONT));

		break;

	case WM_COMMAND:
	{
		if (LOWORD(wParam) == ID_BUTTON_CREATE_FOLDERS)
		{
			bool sortFolder = IsDlgButtonChecked(hwnd, ID_CHECKBOX_SORT_FOLDER);
			bool encryptFolder = IsDlgButtonChecked(hwnd, ID_CHECKBOX_ENCRYPT_FOLDER);

			string diskPath = TextFromComboboxToString(hwndCombo);
			if (!CheckIfStringEmpty(diskPath, L"Choose a disk.", hwnd, NULL))
				break;

			if (sortFolder)
			{
				string strSortFolderPath = diskPath + "Sort";
				wstring wstrSortFolderPath = toWString(strSortFolderPath);
				CreateDirectory(&wstrSortFolderPath[0], NULL);
			}

			if (encryptFolder)
			{
				string strEncryptFolderPath = diskPath + "Encrypt";
				wstring wstrEncryptFolderPath = toWString(strEncryptFolderPath);
				CreateDirectory(&wstrEncryptFolderPath[0], NULL);
			}

			if (!sortFolder && !encryptFolder)
			{
				MessageBox(hwnd, L"Choose a folder.", L"Error", MB_OK);
				break;
			}

			//TODO : return diskPath somehow through global var I guess
			g_diskPath = diskPath;

			DestroyWindow(hwnd);
			break;
		}

		break;
	}
	case WM_CLOSE:

		break;

	case WM_DESTROY:
		break;
	}

	return (DefWindowProcW(hwnd, msg, wParam, lParam));
}

LRESULT CALLBACK DiskDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HWND hwndButtonCreateAndMount, hwndButtonCreate;
	static LPCWSTR userdata;
	static HWND hwndEditDiskSize, hwndEditDiskName, hwndEditFolderPath;

	switch (msg) 
	{
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
			if (!CheckIfStringEmpty(strDiskName, L"Enter disk name.", hwnd, hwndEditDiskName))
				break;
			if (strDiskName.find(".vhd") != strDiskName.size() - 4)
				strDiskName.append(".vhd");

			string strFolderName = WindowTextToString(hwndEditFolderPath);

			string strDiskSize = WindowTextToString(hwndEditDiskSize);
			if (!CheckIfStringEmpty(strDiskSize, L"Enter disk size (in MB).", hwnd, hwndEditDiskSize))
				break;

			if (!CheckIfStringContainsNumbersOnly(strDiskSize, L"Enter a valid disk size - numbers only.", hwnd, hwndEditDiskSize))
				break;

			int size = stoi(strDiskSize);

			string strFullDiskPath = strFolderName + "\\" + strDiskSize;
			wstring wstrFullDiskPath = toWString(strFullDiskPath);

			// TODO : refresh treeview after mount

			//commented out for safety :p
			/*CreateVHD_Fixed(&wstrFullDiskPath[0], size);
			OpenAndAttachVHD2(&wstrFullDiskPath[0], CountPhysicalDisks());*/

			DestroyWindow(hwnd);
			break;
		}

		if (LOWORD(wParam) == ID_BUTTON_CREATE)
		{
			string strDiskName = WindowTextToString(hwndEditDiskName);
			if (!CheckIfStringEmpty(strDiskName, L"Enter disk name.", hwnd, hwndEditDiskName))
				break;

			string strFolderName = WindowTextToString(hwndEditFolderPath);

			string strDiskSize = WindowTextToString(hwndEditDiskSize);
			if (!CheckIfStringEmpty(strDiskSize, L"Enter disk size.", hwnd, hwndEditDiskSize))
				break;
			if (!CheckIfStringContainsNumbersOnly(strDiskSize, L"Enter a valid disk size - numbers only.", hwnd, hwndEditDiskSize))
				break;
			int size = stoi(strDiskSize);

			string strFullDiskPath = strFolderName + "\\" + strDiskSize;
			wstring wstrFullDiskPath = toWString(strFullDiskPath);

			//commented out for safety :p
			//CreateVHD_Fixed(&wstrFullDiskPath[0], size);

			DestroyWindow(hwnd);
			break;
		}

		break;
	}
	case WM_CLOSE:
		break;

	}

	return (DefWindowProcW(hwnd, msg, wParam, lParam));
}

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
	//EnableWindow(hwndParent, FALSE); 
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
	ofn.lpstrFilter = TEXT("Only VHDs(*.*)\0*.VHD\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrInitialDir = L"C:\\";
	ofn.lpstrFileTitle = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn))
	{
		SetWindowText(hwnd, ofn.lpstrFile);
		//commented out for safety

		//OpenAndAttachVHD2(ofn.lpstrFile, CountPhysicalDisks());
	}
}