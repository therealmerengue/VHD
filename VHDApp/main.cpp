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
#define ID_BUTTON_CREATE_FOLDERS 5
#define ID_LABEL 6
#define ID_BUTTON_SORT 17

#define IDM_DISK_NEW 11
#define IDM_DISK_MOUNT 12
#define IDM_QUIT 13

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
	HWND hwndButtonNewDisk, hwndButtonMountDisk, hwndButtonCreateFolders, hwndButtonSort; //buttons left
	std::vector<string> driveLetters = GetDriveLetters();
	std::vector<string> files;
	std::vector<string> dirs;

	switch (msg) 
	{
	case WM_CREATE:

		CenterWindow(hwnd);
		AddMenus(hwnd);

		RegisterDialogClass(hwnd, g_hinst, L"NewDiskDialog", (WNDPROC)NewDiskDialogProc);
		RegisterDialogClass(hwnd, g_hinst, L"CreateFoldersDialog", (WNDPROC)CreateFoldersDialogProc);
		RegisterDialogClass(hwnd, g_hinst, L"ChooseFolderToSortDialog", (WNDPROC)ChooseFolderToSortDialogProc);

		//Buttons left

		hwndButtonNewDisk = CreateWindowW(L"button", L"New disk",
			WS_VISIBLE | WS_CHILD, 10, 20, 90, 25,
			hwnd, (HMENU)ID_BUTTON_NEW_DISK, NULL, NULL);

		hwndButtonMountDisk = CreateWindowW(L"button", L"Mount disk",
			WS_VISIBLE | WS_CHILD, 10, 50, 90, 25,
			hwnd, (HMENU)ID_BUTTON_MOUNT_DISK, NULL, NULL);

		hwndButtonCreateFolders = CreateWindowW(L"button", L"Create folders",
			WS_VISIBLE | WS_CHILD, 10, 80, 90, 25,
			hwnd, (HMENU)ID_BUTTON_CREATE_FOLDERS, NULL, NULL);

		hwndButtonSort = CreateWindowW(L"button", L"Sort",
			WS_VISIBLE | WS_CHILD, 10, 110, 90, 25,
			hwnd, (HMENU)ID_BUTTON_SORT, NULL, NULL);

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

		if (LOWORD(wParam) == IDM_DISK_NEW) 
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

		if (LOWORD(wParam) == IDM_DISK_MOUNT) 
		{
			OpenFileDialog(hwnd);
			break;
		}

		if (LOWORD(wParam == ID_BUTTON_NEW_DISK)) 
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

		if (LOWORD(wParam == ID_BUTTON_MOUNT_DISK)) 
		{
			OpenFileDialog(hwnd);
			break;
		}

		if (LOWORD(wParam == ID_BUTTON_CREATE_FOLDERS)) 
		{
			HWND dialog = CreateDialogBox(hwnd, g_hinst, NULL, L"CreateFoldersDialog", L"Create folders");
			CenterWindow(dialog); 
			break;
		}

		if (LOWORD(wParam == ID_BUTTON_SORT)) 
		{
			//sort created folder
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
				//show choose folder to sort dialog
				HWND dialog = CreateDialogBox(hwnd, g_hinst, NULL, L"ChooseFolderToSortDialog", L"Choose folder to sort");
				CenterWindow(dialog);
			}
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
		break;

	case WM_KILLFOCUS:
		break;

	case WM_DESTROY:

		PostQuitMessage(0);

		break;
	}

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void AddMenus(HWND hwnd) 
{
	HMENU hMenubar;
	HMENU hMenu;

	hMenubar = CreateMenu();
	hMenu = CreateMenu();

	AppendMenuW(hMenu, MF_STRING, IDM_DISK_NEW, L"&New disk");
	AppendMenuW(hMenu, MF_STRING, IDM_DISK_MOUNT, L"&Mount disk");
	AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
	AppendMenuW(hMenu, MF_STRING, IDM_QUIT, L"&Quit");

	AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hMenu, L"&File");
	SetMenu(hwnd, hMenubar);
}