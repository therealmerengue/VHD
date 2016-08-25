#include <windows.h>
#include <ShlObj.h>
#include <commoncontrols.h>
#include <string>

#include "VHD.h"
#include "Treeview.h"
#include "Files.h"
#include "Dialogs.h"
#include "Combobox.h"

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define ID_EDIT 1
#define ID_BUTTON_NEW_DISK 2
#define ID_BUTTON_MOUNT_DISK 3
#define ID_BUTTON_CHOOSE_DISK 5
#define ID_LABEL 6
#define ID_BUTTON_SORT 17
#define ID_BUTTON_ENCRYPT 19

#define IDM_DISK_NEW 40
#define IDM_DISK_MOUNT 41
#define IDM_QUIT 42
#define IDM_DISK_CHOOSE 43
#define IDM_SORT 44
#define IDM_ENCRYPT 45

#define ID_BUTTON_TEST 99

HANDLE hFont = CreateFont(20, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void AddMenus(HWND hwnd);

UINT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);

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
	hwnd = CreateWindowW(wc.lpszClassName, L"Window",
		WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,
		100, 100, 800, 600, NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) 
{
	static HWND hwndTreeView;
	HWND hwndButtonNewDisk, hwndButtonMountDisk, hwndButtonChooseDisk, hwndButtonSort, hwndButtonEncrypt; //buttons left
	HWND hwndButtonTest;
	std::vector<string> driveLetters = GetDriveLetters();
	std::vector<string> files;
	std::vector<string> dirs;

	switch (msg) 
	{
	case WM_CREATE:

		CenterWindow(hwnd);
		AddMenus(hwnd);

		RegisterDialogClass(hwnd, g_hinst, L"NewDiskDialog", (WNDPROC)NewDiskDialogProc);
		RegisterDialogClass(hwnd, g_hinst, L"CreateFoldersDialog", (WNDPROC)ChooseDiskDialogProc);
		RegisterDialogClass(hwnd, g_hinst, L"NoDiskChosen", (WNDPROC)NoFoldersCreatedDialogProc);

		//Buttons left

		hwndButtonNewDisk = CreateWindowW(L"button", L"New disk",
			WS_VISIBLE | WS_CHILD, 10, 20, 90, 25,
			hwnd, (HMENU)ID_BUTTON_NEW_DISK, NULL, NULL);

		hwndButtonMountDisk = CreateWindowW(L"button", L"Mount disk",
			WS_VISIBLE | WS_CHILD, 10, 50, 90, 25,
			hwnd, (HMENU)ID_BUTTON_MOUNT_DISK, NULL, NULL);

		hwndButtonChooseDisk = CreateWindowW(L"button", L"Choose disk",
			WS_VISIBLE | WS_CHILD, 10, 80, 90, 25,
			hwnd, (HMENU)ID_BUTTON_CHOOSE_DISK, NULL, NULL);

		hwndButtonSort = CreateWindowW(L"button", L"Sort",
			WS_VISIBLE | WS_CHILD, 10, 110, 90, 25,
			hwnd, (HMENU)ID_BUTTON_SORT, NULL, NULL);

		hwndButtonEncrypt = CreateWindowW(L"button", L"Encrypt",
			WS_VISIBLE | WS_CHILD, 10, 140, 90, 25,
			hwnd, (HMENU)ID_BUTTON_ENCRYPT, NULL, NULL);

		hwndButtonTest = CreateWindowW(L"button", L"Test",
			WS_VISIBLE | WS_CHILD, 10, 170, 90, 25,
			hwnd, (HMENU)ID_BUTTON_TEST, NULL, NULL);

		//File treeview - center

		hwndTreeView = CreateATreeView(g_hinst, hwnd, 225, 16, 335, 250);
		AddItemsToTreeView(driveLetters, hwndTreeView);
		SetImageList(hwndTreeView);

		//Debug edit text
		
		static HWND hwndDebug = CreateWindowW(L"Edit", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
			400, 500, 150, 20, hwnd, (HMENU)ID_EDIT,
			NULL, NULL);

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
				HWND dialog = CreateDialogBox(hwnd, g_hinst, folderPath, L"NewDiskDialog", L"Create disk");
				CenterWindow(dialog);
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
			HWND dialog = CreateDialogBox(hwnd, g_hinst, NULL, L"CreateFoldersDialog", L"Choose disk");
			CenterWindow(dialog); 
			break;
		}

		if (LOWORD(wParam == IDM_QUIT))
		{
			DestroyWindow(hwnd);
		}

		if (LOWORD(wParam == ID_BUTTON_SORT) || LOWORD(wParam == IDM_SORT)) 
		{
			if (!g_diskPath.empty())
			{
				vector<string> filesToSort;
				string folderToSort = g_diskPath + "Sort";
				//commented out for safety
				/*GetFilesInDirectory(folderToSort.c_str(), filesToSort, vector<string>());
				Sort(filesToSort, g_diskPath, folderToSort);*/
			}
			else
			{
				//show choose disk dialog
				HWND dialog = CreateDialogBox(hwnd, g_hinst, NULL, L"NoDiskChosen", L"Error");
				CenterWindow(dialog);
			}

			break;
		}

		if (LOWORD(wParam == ID_BUTTON_ENCRYPT) || LOWORD(wParam == IDM_ENCRYPT))
		{
			if (!g_diskPath.empty())
			{
				//encrypt
			}
			else
			{
				//show choose disk dialog
				HWND dialog = CreateDialogBox(hwnd, g_hinst, NULL, L"NoDiskChosen", L"Error");
				CenterWindow(dialog);
			}

			break;
		}

		if (LOWORD(wParam == ID_BUTTON_TEST))
		{
			DialogBox(g_hinst, NULL, hwnd, (DLGPROC)DialogProc);
			DWORD err = GetLastError();
			wchar_t szTest[10];
			swprintf_s(szTest, 10, L"%d", err);
			MessageBox(hwnd, szTest, L"error", MB_OK);
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
		return 0;
	}

	case WM_SETFOCUS:
		EnableWindow(hwnd, TRUE);
		ShowWindow(hwnd, SW_RESTORE);
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

	AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hMenuFile, L"&File");

	AppendMenuW(hMenuFile, MF_STRING, IDM_DISK_NEW, L"&New disk");
	AppendMenuW(hMenuFile, MF_STRING, IDM_DISK_MOUNT, L"&Mount disk");
	AppendMenuW(hMenuFile, MF_STRING, IDM_DISK_CHOOSE, L"&Choose disk");
	AppendMenuW(hMenuFile, MF_SEPARATOR, 0, NULL);
	AppendMenuW(hMenuFile, MF_STRING, IDM_QUIT, L"&Quit");

	AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hMenuAction, L"&Action");
	AppendMenuW(hMenuAction, MF_STRING, IDM_SORT, L"&Sort");
	AppendMenuW(hMenuAction, MF_STRING, IDM_ENCRYPT, L"&Encrypt");
	
	SetMenu(hwnd, hMenubar);
}

UINT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_INITDIALOG:

		break;
	case WM_COMMAND:

		break;
	default:
		return FALSE;
	}
	return TRUE;
}