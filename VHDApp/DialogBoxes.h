#pragma once
#include <Windows.h>
#include "resources.h"
#include "StringOperations.h"

using namespace std;

string g_diskPath;
HINSTANCE g_hinst;

bool SetupDiskCreation(HWND hwndDlg, wstring& wstrFullDiskPath)
{
	HWND hwndEditDiskName = GetDlgItem(hwndDlg, ID_EDIT_DISKNAME);
	string strDiskName = WindowTextToString(hwndEditDiskName);
	if (!CheckIfStringEmpty(strDiskName, L"Enter disk name.", hwndDlg, hwndEditDiskName))
		return false;
	if (strDiskName.find(".vhd") != strDiskName.size() - 4)
		strDiskName.append(".vhd");

	HWND hwndEditDiskSize = GetDlgItem(hwndDlg, ID_EDIT_DISKSIZE);
	string strDiskSize = WindowTextToString(hwndEditDiskSize);
	if (!CheckIfStringEmpty(strDiskSize, L"Enter disk size (in MB).", hwndDlg, hwndEditDiskSize))
		return false;
	if (!CheckIfStringContainsNumbersOnly(strDiskSize, L"Enter a valid disk size - numbers only.", hwndDlg, hwndEditDiskSize))
		return false;
	int size = stoi(strDiskSize);

	HWND hwndEditFolderPath = GetDlgItem(hwndDlg, ID_EDIT_DISKFOLDER);

	string strFolderName = WindowTextToString(hwndEditFolderPath);

	string strFullDiskPath = strFolderName + "\\" + strDiskName;
	wstrFullDiskPath = toWString(strFullDiskPath);

	return true;
}

INT_PTR CALLBACK NewDiskDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static LPCWSTR path;
	static HWND hwndEditFolderPath;
	switch (uMsg)
	{
	case WM_INITDIALOG:
		path = (LPCWSTR)lParam;
		hwndEditFolderPath = GetDlgItem(hwndDlg, ID_EDIT_DISKFOLDER);
		SetWindowText(hwndEditFolderPath, path);
		EnumChildWindows(hwndDlg, (WNDENUMPROC)SetFont, (LPARAM)GetStockObject(DEFAULT_GUI_FONT));
		break;

	case WM_COMMAND:

		if (LOWORD(wParam == ID_BTN_CREATE_AND_MOUNT))
		{
			wstring wstrFullDiskPath;
			if (!SetupDiskCreation(hwndDlg, wstrFullDiskPath))
				break;
			//commented out for safety :p
			/*CreateVHD_Fixed(&wstrFullDiskPath[0], size);
			OpenAndAttachVHD2(&wstrFullDiskPath[0], CountPhysicalDisks());*/
			EndDialog(hwndDlg, ID_BTN_CREATE_AND_MOUNT);

			break;
		}

		if (LOWORD(wParam == ID_BTN_CREATE))
		{
			wstring wstrFullDiskPath;
			if (!SetupDiskCreation(hwndDlg, wstrFullDiskPath))
				break;
			//commented out for safety :p
			//CreateVHD_Fixed(&wstrFullDiskPath[0], size);
			EndDialog(hwndDlg, ID_BTN_CREATE_AND_MOUNT);
		}

		break;

	case WM_CLOSE:
		EndDialog(hwndDlg, 0);
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

INT_PTR CALLBACK ChooseDiskDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndCombo;

	switch (uMsg)
	{
	case WM_INITDIALOG:
		hwndCombo = GetDlgItem(hwndDlg, ID_COMBO_DISK);
		AddItemsToCombobox(hwndCombo, GetDriveLetters());
		EnumChildWindows(hwndDlg, (WNDENUMPROC)SetFont, (LPARAM)GetStockObject(DEFAULT_GUI_FONT));
		break;

	case WM_COMMAND:

		if (LOWORD(wParam) == ID_BTN_CREATE_FOLDERS)
		{
			hwndCombo = GetDlgItem(hwndDlg, ID_COMBO_DISK);
			string diskPath = TextFromComboboxToString(hwndCombo);
			if (!CheckIfStringEmpty(diskPath, L"Choose a disk.", hwndDlg, NULL))
				break;

			string strSortFolderPath = diskPath + "Sort";
			wstring wstrSortFolderPath = toWString(strSortFolderPath);
			CreateDirectory(&wstrSortFolderPath[0], NULL);

			string strEncryptFolderPath = diskPath + "Encrypt";
			wstring wstrEncryptFolderPath = toWString(strEncryptFolderPath);
			CreateDirectory(&wstrEncryptFolderPath[0], NULL);

			//TODO : return diskPath somehow through global var I guess
			g_diskPath = diskPath;

			EndDialog(hwndDlg, ID_BTN_CREATE_FOLDERS);
			break;
		}

		break;

	case WM_CLOSE:
		EndDialog(hwndDlg, 0);
		break;

	case WM_QUIT:
		EndDialog(hwndDlg, 0);
		PostQuitMessage(0);
		break;

	case WM_DESTROY:
		EndDialog(hwndDlg, 0);
		break;

	default:
		return FALSE;
	}
	return TRUE;
}

INT_PTR CALLBACK NoFoldersDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		EnumChildWindows(hwndDlg, (WNDENUMPROC)SetFont, (LPARAM)GetStockObject(DEFAULT_GUI_FONT));
		break;

	case WM_COMMAND:
		if (LOWORD(wParam) == ID_BTN_CHOOSE_DISK)
		{
			DialogBox(g_hinst, MAKEINTRESOURCE(IDD_CHOOSEDISK), hwndDlg, (DLGPROC)ChooseDiskDialogProc);
			EndDialog(hwndDlg, 0);
			break;
		}

		if (LOWORD(wParam) == ID_BTN_CANCEL)
		{
			EndDialog(hwndDlg, 0);
			break;
		}
		
		break;

	case WM_CLOSE:
		EndDialog(hwndDlg, 0);
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

INT_PTR CALLBACK EncryptDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndEditPassword;
	switch (uMsg)
	{
	case WM_INITDIALOG:

		break;

	case WM_COMMAND:

		if (LOWORD(wParam == ID_BTN_ENCRYPT))
		{
			vector<string> filesToEncrypt;
			GetFilesInDirectory((g_diskPath + "Encrypt").c_str(), filesToEncrypt, vector<string>());
			hwndEditPassword = GetDlgItem(hwndDlg, ID_EDIT_PASSWORD);
			string password = WindowTextToString(hwndEditPassword);
			CreateDirectory(s2ws(g_diskPath + "\\Encrypted").c_str(), NULL);
			EncryptFiles(filesToEncrypt, g_diskPath + "Encrypt", g_diskPath + "Encrypted", password);
			EndDialog(hwndDlg, ID_BTN_ENCRYPT); // TODO : check if encrypting files works, take away choosing folders to create -> create all anyway
		}

		break;

	case WM_CLOSE:
		EndDialog(hwndDlg, 0);
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