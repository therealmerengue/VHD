#include <windows.h>
#include <ShlObj.h>
#include <commoncontrols.h>
#include <string>

#include <thread>
#include <chrono>
#include <ctime>

#include "VHD.h"
#include "Treeview.h"
#include "Files.h"
#include "Dialogs.h"
#include "Combobox.h"

#include "resources.h"
#include "DialogBoxes.h"

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "comctl32.lib")

HANDLE hFont = CreateFont(20, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void AddMenus(HWND hwnd);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PWSTR pCmdLine, int nCmdShow) 
{
	MSG  msg;
	HWND hwnd;
	WNDCLASSW wc;

	wc.style = CS_HREDRAW | CS_VREDRAW; // window is redrawn on move/resize
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.lpszClassName = L"Window";
	wc.hInstance = hInstance; // setting instance of the program
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpszMenuName = NULL;
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassW(&wc);
	hwnd = CreateWindowW(wc.lpszClassName, L"VHD App",
		WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,
		100, 100, 575, 355, NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) 
{
	static HWND hwndTreeView;
	HWND hwndButtonNewDisk, hwndButtonMountDisk, hwndButtonChooseDisk, hwndButtonSort, hwndButtonEncrypt, hwndButtonDecrypt, hwndButtonDetachDisk; //buttons right
	std::vector<string> driveLetters = GetDriveLetters();
	std::vector<string> files;
	std::vector<string> dirs;
	
	switch (msg)
	{
	case WM_CREATE:

		CenterWindow(hwnd);
		AddMenus(hwnd);

		//Buttons left

		hwndButtonNewDisk = CreateWindowW(L"button", L"New disk",
			WS_VISIBLE | WS_CHILD, 375, 20, 150, 25,
			hwnd, (HMENU)ID_BUTTON_NEW_DISK, NULL, NULL);

		hwndButtonMountDisk = CreateWindowW(L"button", L"Mount disk",
			WS_VISIBLE | WS_CHILD, 375, 50, 150, 25,
			hwnd, (HMENU)ID_BUTTON_MOUNT_DISK, NULL, NULL);

		hwndButtonChooseDisk = CreateWindowW(L"button", L"Choose disk",
			WS_VISIBLE | WS_CHILD, 375, 80, 150, 25,
			hwnd, (HMENU)ID_BUTTON_CHOOSE_DISK, NULL, NULL);

		hwndButtonSort = CreateWindowW(L"button", L"Sort",
			WS_VISIBLE | WS_CHILD, 375, 110, 150, 25,
			hwnd, (HMENU)ID_BUTTON_SORT, NULL, NULL);

		hwndButtonEncrypt = CreateWindowW(L"button", L"Encrypt",
			WS_VISIBLE | WS_CHILD, 375, 140, 150, 25,
			hwnd, (HMENU)ID_BUTTON_ENCRYPT, NULL, NULL);

		hwndButtonDecrypt = CreateWindowW(L"button", L"Decrypt",
			WS_VISIBLE | WS_CHILD, 375, 170, 150, 25,
			hwnd, (HMENU)ID_BUTTON_DECRYPT, NULL, NULL);

		hwndButtonDetachDisk = CreateWindowW(L"button", L"Detach disk",
			WS_VISIBLE | WS_CHILD, 375, 200, 150, 25,
			hwnd, (HMENU)ID_BUTTON_DETACH_DISK, NULL, NULL);

		//File treeview - center

		hwndTreeView = CreateATreeView(g_hinst, hwnd, 30, 20, 335, 250);
		AddItemsToTreeView(driveLetters, hwndTreeView);
		SetImageList(hwndTreeView);

		EnumChildWindows(hwnd, (WNDENUMPROC)SetFont, (LPARAM)GetStockObject(DEFAULT_GUI_FONT));

		break;

	case WM_COMMAND:

		if (LOWORD(wParam) == IDM_DISK_NEW || LOWORD(wParam == ID_BUTTON_NEW_DISK))
		{
			wchar_t folderPath[MAX_PATH + 1];
			auto pidl = OpenFolderDialog(hwnd);
			if (pidl) 
			{
				SHGetPathFromIDList(pidl, folderPath);
				SetWindowText(hwnd, folderPath);
				DialogBoxParam(g_hinst, MAKEINTRESOURCE(IDD_NEWDISK), hwnd, (DLGPROC)NewDiskDialogProc, (LPARAM)folderPath);
			}
			break;
		}

		if (LOWORD(wParam) == IDM_DISK_MOUNT || LOWORD(wParam == ID_BUTTON_MOUNT_DISK))
		{
			OpenFileDialog(hwnd);
			break;
		}

		if (LOWORD(wParam == ID_BUTTON_CHOOSE_DISK) || LOWORD(wParam == IDM_DISK_CHOOSE))
		{
			DialogBox(g_hinst, MAKEINTRESOURCE(IDD_CHOOSEDISK), hwnd, (DLGPROC)ChooseDiskDialogProc);
			break;
		}

		if (LOWORD(wParam == IDM_QUIT))
		{
			DestroyWindow(hwnd);
		}

		if (LOWORD(wParam == ID_BUTTON_SORT)) 
		{
			if (!g_diskPath.empty())
			{
				HTREEITEM selectedFolder = TreeView_GetSelection(hwndTreeView);
				string folderToSort = GetFullNodePath(hwndTreeView, selectedFolder);

				if (!selectedFolder)
				{
					MessageBox(hwnd, L"Choose a folder to sort from treeview and press Sort.", L"Error", MB_OK);
					break;
				}

				if (IsDirectory(folderToSort.c_str()) == 0)
				{
					MessageBox(hwnd, L"Choose a directory, not a file.", L"Error", MB_OK);
					break;
				}

				PerformSort(hwnd, folderToSort, g_diskPath);
			}
			else
			{
				//show choose disk dialog
				DialogBox(g_hinst, MAKEINTRESOURCE(IDD_NOFOLDERS), hwnd, (DLGPROC)NoFoldersDialogProc);
			}

			break;
		}

		if (LOWORD(wParam) == IDM_SORT)
		{
			if (!g_diskPath.empty())
			{
				PerformSort(hwnd, g_diskPath + "Sort", g_diskPath);
			}
			else
			{
				//show choose disk dialog
				DialogBox(g_hinst, MAKEINTRESOURCE(IDD_NOFOLDERS), hwnd, (DLGPROC)NoFoldersDialogProc);
			}
		}

		if (LOWORD(wParam == ID_BUTTON_ENCRYPT))
		{
			if (!g_diskPath.empty())
			{
				//encrypt
				DialogBoxParam(g_hinst, MAKEINTRESOURCE(IDD_ENCRYPT), hwnd, (DLGPROC)EncryptDialogProc, (LPARAM)hwndTreeView);
			}
			else
			{
				//show choose disk dialog
				DialogBox(g_hinst, MAKEINTRESOURCE(IDD_NOFOLDERS), hwnd, (DLGPROC)NoFoldersDialogProc);
			}

			break;
		}

		if (LOWORD(wParam == IDM_ENCRYPT))
		{
			if (!g_diskPath.empty())
			{
				//encrypt
				DialogBoxParam(g_hinst, MAKEINTRESOURCE(IDD_ENCRYPT), hwnd, (DLGPROC)EncryptDialogProc, NULL);
			}
			else
			{
				//show choose disk dialog
				DialogBox(g_hinst, MAKEINTRESOURCE(IDD_NOFOLDERS), hwnd, (DLGPROC)NoFoldersDialogProc);
			}

			break;
		}

		if (LOWORD(wParam == ID_BUTTON_DECRYPT) || LOWORD(wParam == IDM_DECRYPT))
		{
			if (!g_diskPath.empty())
			{
				//encrypt
				DialogBox(g_hinst, MAKEINTRESOURCE(IDD_DECRYPT), hwnd, (DLGPROC)DecryptDialogProc);
			}
			else
			{
				//show choose disk dialog
				DialogBox(g_hinst, MAKEINTRESOURCE(IDD_NOFOLDERS), hwnd, (DLGPROC)NoFoldersDialogProc);
			}

			break;
		}

		if (LOWORD(wParam == ID_BUTTON_DETACH_DISK) || LOWORD(wParam == IDM_DISK_DETACH))
		{
			OpenFileDialog(hwnd, DETACH_DISK);
		}

		break;

	case WM_NOTIFY:
	{
		HTREEITEM hitem;

		LPNM_TREEVIEW pntv = (LPNM_TREEVIEW)lParam;

		if (pntv->hdr.code == TVN_SELCHANGED)
		{
			WCHAR buffer[128];
			TVITEM item = GetSelectedNode(hwnd, hwndTreeView, pntv, buffer);
			std::string fullNodePath = GetFullNodePath(hwndTreeView, item.hItem);

			if (!TreeView_GetChild(hwndTreeView, item.hItem))
			{
				files.clear();
				dirs.clear();
				GetFilesInDirectory(fullNodePath.c_str(), files, dirs);

				for (size_t i = 0; i < files.size(); i++)
				{
					wstring wstr = toWString(files[i]);
					AddItemToParent(hwndTreeView, &wstr[0], item.hItem);
				}

				for (size_t i = 0; i < dirs.size(); i++)
				{
					wstring wstr = toWString(dirs[i]);
					AddItemToParent(hwndTreeView, &wstr[0], item.hItem, 3, 4);
				}
			}
		}

		break;
	}

	case WM_SETFOCUS:
		EnableWindow(hwnd, TRUE);
		ShowWindow(hwnd, SW_RESTORE);
		TreeView_DeleteAllItems(hwndTreeView);
		AddItemsToTreeView(GetDriveLetters(), hwndTreeView);
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		DestroyWindow(hwnd);
		PostQuitMessage(0);
		break;
	}

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void AddMenus(HWND hwnd) 
{
	HMENU hMenubar;
	HMENU hMenuFile, hMenuAction;

	hMenubar = CreateMenu();
	hMenuFile = CreateMenu();
	hMenuAction = CreateMenu();

	AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hMenuFile, L"&Disks");

	AppendMenuW(hMenuFile, MF_STRING, IDM_DISK_NEW, L"&New disk");
	AppendMenuW(hMenuFile, MF_STRING, IDM_DISK_MOUNT, L"&Mount disk");
	AppendMenuW(hMenuFile, MF_STRING, IDM_DISK_CHOOSE, L"&Choose disk");
	AppendMenuW(hMenuFile, MF_SEPARATOR, 0, NULL);
	AppendMenuW(hMenuFile, MF_STRING, IDM_DISK_DETACH, L"&Detach disk");
	AppendMenuW(hMenuFile, MF_STRING, IDM_QUIT, L"&Quit");

	AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hMenuAction, L"&Disk Folders");
	AppendMenuW(hMenuAction, MF_STRING, IDM_SORT, L"&Sort");
	AppendMenuW(hMenuAction, MF_STRING, IDM_ENCRYPT, L"&Encrypt");
	AppendMenuW(hMenuAction, MF_STRING, IDM_DECRYPT, L"&Decrypt");
	
	SetMenu(hwnd, hMenubar);
}