#pragma once
#include <Windows.h>
#include "resources.h"
#include "StringOperations.h"

using namespace std;

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

UINT_PTR CALLBACK NewDiscDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static LPCWSTR path;
	static HWND hwndEditFolderPath;
	switch (uMsg)
	{
	case WM_INITDIALOG:
		path = (LPCWSTR)lParam;
		hwndEditFolderPath = GetDlgItem(hwndDlg, ID_EDIT_DISKFOLDER);
		SetWindowText(hwndEditFolderPath, path);
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