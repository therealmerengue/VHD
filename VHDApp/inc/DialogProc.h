#pragma once

extern std::string g_diskPath;
extern HINSTANCE g_hinst;

namespace DialogProc
{
	bool SetupDiskCreation(HWND hwndDlg, std::wstring& wstrFullDiskPath, int& size);

	INT_PTR CALLBACK NewDiskDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	INT_PTR CALLBACK ChooseDiskDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	INT_PTR CALLBACK NoFoldersDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	INT_PTR CALLBACK EncryptDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	INT_PTR CALLBACK DecryptDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
}
