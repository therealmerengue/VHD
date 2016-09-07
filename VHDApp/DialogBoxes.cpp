#pragma once
#include <Windows.h>
#include <chrono>
#include <CommCtrl.h>
#include "resources.h"
#include "StringOperations.h"
#include "Dialogs.h"
#include "Treeview.h"
#include "Combobox.h"
#include "DialogBoxes.h"
#include "Files.h"
#include "Visuals.h"
#include "VHD.h"
#include "Treeview.h"

#include <string>

using namespace std;

std::string g_diskPath;
HINSTANCE g_hinst;

bool SetupDiskCreation(HWND hwndDlg, wstring& wstrFullDiskPath, int& size)
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
	size = stoi(strDiskSize);

	HWND hwndEditFolderPath = GetDlgItem(hwndDlg, ID_EDIT_DISKFOLDER);

	string strFolderName = WindowTextToString(hwndEditFolderPath);

	string strFullDiskPath = strFolderName + "\\" + strDiskName;
	wstrFullDiskPath = toWString(strFullDiskPath);

	return true;
}

INT_PTR CALLBACK NewDiskDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static LPCWSTR path;
	static HWND hwndEditFolderPath, hwndEditDiskSize;
	int size = 0;

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
			if (!SetupDiskCreation(hwndDlg, wstrFullDiskPath, size))
				break;
			
			CreateVHD_Fixed(&wstrFullDiskPath[0], size);
			OpenAndAttachVHD2(&wstrFullDiskPath[0], CountPhysicalDisks());
			EndDialog(hwndDlg, ID_BTN_CREATE_AND_MOUNT);

			break;
		}

		if (LOWORD(wParam == ID_BTN_CREATE))
		{
			wstring wstrFullDiskPath;
			if (!SetupDiskCreation(hwndDlg, wstrFullDiskPath, size))
				break;

			CreateVHD_Fixed(&wstrFullDiskPath[0], size);
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

			string strDecryptFolderPath = diskPath + "Decrypt";
			wstring wstrDecryptFolderPath = toWString(strDecryptFolderPath);
			CreateDirectory(&wstrDecryptFolderPath[0], NULL);

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
	static HWND hwndTV;
	static HWND hwndEditPassword;
	
	switch (uMsg)
	{
	case WM_INITDIALOG:
		hwndEditPassword = GetDlgItem(hwndDlg, ID_EDIT_ENCRYPT_PASSWORD);
		hwndTV = (HWND)lParam;
		CreateDirectory(s2ws(g_diskPath + "\\Encrypted").c_str(), NULL);
		break;

	case WM_COMMAND:

		if (LOWORD(wParam == ID_BTN_ENCRYPT))
		{
			if (!hwndTV)
			{
				vector<string> filesToEncrypt;
				GetFilesInDirectory((g_diskPath + "Encrypt").c_str(), filesToEncrypt, vector<string>()); 
				std::chrono::steady_clock::time_point begin_time = std::chrono::steady_clock::now();
				string password = WindowTextToString(hwndEditPassword);
				MessageBox(hwndDlg, &toWString(password)[0], L"Suh dude", 0);
				EncryptFiles(filesToEncrypt, g_diskPath + "Encrypt", g_diskPath + "Encrypted", password);
				std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
				ShowTimeDialog(hwndDlg, begin_time, end_time, L"Encrypted in: ");
				EndDialog(hwndDlg, ID_BTN_ENCRYPT); 
			}
			else
			{
				HTREEITEM selectedFile = TreeView_GetSelection(hwndTV);
				string fileToEncrypt = GetFullNodePath(hwndTV, selectedFile);
				wstring fileName = GetItemText(hwndTV, selectedFile);
				std::chrono::steady_clock::time_point begin_time = std::chrono::steady_clock::now();
				string password = WindowTextToString(hwndEditPassword);
				MessageBox(hwndDlg, &toWString(password)[0], L"Suh dude", 0);
				CreateEncryptedFile(fileToEncrypt, g_diskPath + "Encrypted" + "\\Crypt_" + toString(fileName), password);
				std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
				ShowTimeDialog(hwndDlg, begin_time, end_time, L"Encrypted in: ");
				EndDialog(hwndDlg, ID_BTN_ENCRYPT);
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

INT_PTR CALLBACK DecryptDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndEditPassword;
	static HWND hwndTV;
	
	switch (uMsg)
	{
	case WM_INITDIALOG:
		hwndEditPassword = GetDlgItem(hwndDlg, ID_EDIT_DECRYPT_PASSWORD);
		hwndTV = (HWND)lParam;
		CreateDirectory(s2ws(g_diskPath + "\\Decrypted").c_str(), NULL);
		break;

	case WM_COMMAND:

		if (LOWORD(wParam == ID_BTN_DECRYPT))
		{
			if (!hwndTV)
			{
				vector<string> filesToDecrypt;
				GetFilesInDirectory((g_diskPath + "Decrypt").c_str(), filesToDecrypt, vector<string>());
				std::chrono::steady_clock::time_point begin_time = std::chrono::steady_clock::now();
				string password = WindowTextToString(hwndEditPassword);
				EncryptFiles(filesToDecrypt, g_diskPath + "Decrypt", g_diskPath + "Decrypted", password);
				std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
				ShowTimeDialog(hwndDlg, begin_time, end_time, L"Decrypted in: ");
				EndDialog(hwndDlg, ID_BTN_DECRYPT);
			}
			else
			{
				HTREEITEM selectedFile = TreeView_GetSelection(hwndTV);
				string fileToDecrypt = GetFullNodePath(hwndTV, selectedFile);
				wstring fileName = GetItemText(hwndTV, selectedFile);
				std::chrono::steady_clock::time_point begin_time = std::chrono::steady_clock::now();
				string password = WindowTextToString(hwndEditPassword);
				CreateEncryptedFile(fileToDecrypt, g_diskPath + "Decrypted" + "\\Crypt_" + toString(fileName), password);
				std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
				ShowTimeDialog(hwndDlg, begin_time, end_time, L"Decrypted in: ");
				EndDialog(hwndDlg, ID_BTN_DECRYPT);
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



