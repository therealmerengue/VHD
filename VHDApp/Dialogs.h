#pragma once
#include <Windows.h>
#include "Visuals.h"
#include "StringOperations.h"
#include "Combobox.h"

size_t GetFileSize(const std::string& filename) {
	struct stat st;
	if (stat(filename.c_str(), &st) != 0) {
		return 0;
	}
	return st.st_size;
}

INT_PTR CALLBACK ExpandDiskDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		static HWND hwndEditExpandSize = GetDlgItem(hwndDlg, ID_EDIT_EXPANDDISKSIZE);
		static LPCWSTR diskFile = (LPCWSTR)lParam;
		EnumChildWindows(hwndDlg, (WNDENUMPROC)SetFont, (LPARAM)GetStockObject(DEFAULT_GUI_FONT));
		break;

	case WM_COMMAND:
		if (LOWORD(wParam) == ID_BTN_EXPAND_DISK)
		{
			string strSize = WindowTextToString(hwndEditExpandSize);
			if (!CheckIfStringContainsNumbersOnly(strSize, L"Enter a valid disk size.", hwndDlg, hwndEditExpandSize))
				break;
			int size = stoi(strSize);
			string strDiskFile = toString(wstring(diskFile));
			int currentSize = GetFileSize(strDiskFile) / pow(1024, 2);
			//MessageBox(hwndDlg, &toWString(std::to_string(currentSize))[0], L"yo", 0);
			if (currentSize > size)
			{
				MessageBox(hwndDlg, L"Entered size was smaller than current size.", L"Error", MB_OK);
				break;
			}

			if (diskFile)
			{
				OpenAndExpandVHD(diskFile, size);
				EndDialog(hwndDlg, ID_BTN_EXPAND_DISK);
			}
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

void OpenFileDialog(HWND hwnd, OpenFileDialogAction action = ATTACH_DISK)
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
		if (action == ATTACH_DISK)
			OpenAndAttachVHD2(ofn.lpstrFile, CountPhysicalDisks());
		if (action == EXPAND_DISK)
		{
			DialogBoxParam(NULL, MAKEINTRESOURCE(IDD_EXPANDDISK), hwnd, (DLGPROC)ExpandDiskDialogProc, (LPARAM)ofn.lpstrFile);
		}
	}
}

void ShowTimeDialog(HWND hwnd, std::chrono::steady_clock::time_point begin_time, std::chrono::steady_clock::time_point end_time, wstring wstrMessageBeginning)
{
	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time).count();
	wstring wstrTime = to_wstring(time);
	wstring wstrMessage = wstrMessageBeginning + wstrTime + L"ms";
	MessageBox(hwnd, &wstrMessage[0], L"Sorted", MB_OK);
}

