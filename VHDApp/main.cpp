#include <windows.h>
#include <ShlObj.h>

#include "VHD.h"
#include "Treeview.h"

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define ID_EDIT 1
#define ID_BUTTON_OPEN_FILE 2
#define ID_BUTTON_OPEN_FOLDER 3
#define ID_BUTTON_CREATE_AND_MOUNT 4
#define ID_BUTTON_MOUNT 5
#define ID_BUTTON_CHOOSE_DISK 8
#define ID_LABEL 6

HINSTANCE g_hinst;
HANDLE hFont = CreateFont(20, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void CenterWindow(HWND hwnd);
void OpenFileDialog(HWND hwnd);
void OpenFolderDialog(HWND hwnd);
bool CALLBACK SetFont(HWND child, LPARAM font);
void AddItemsToCombobox(HWND combobox, const std::vector<std::string>& items);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PWSTR pCmdLine, int nCmdShow) {

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

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	static HWND hwndEditChooseDisk, hwndCombo, hwndEditDiskName, hwndEditDiskFolder, hwndEditDiskSize, hwndTreeView;
	HWND hwndButtonOpenFile, hwndButtonBrowseFolders, hwndButtonCreateAndMount, hwndButtonMount, hwndButtonChooseDisk;

	int size = 0;
	std::vector<string> driveLetters = getDriveLetters(size);

	switch (msg) {

	case WM_CREATE:

		CenterWindow(hwnd);

		//Create disk groupbox - left top

		CreateWindowW(L"Button", L"Create disk",
			WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
			10, 10, 210, 135, hwnd, (HMENU)0, g_hinst, NULL);

		hwndEditDiskName = CreateWindowW(L"Edit", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			55, 30, 150, 20, hwnd, (HMENU)ID_EDIT,
			NULL, NULL);

		CreateWindowW(L"static", L"Name:",
			WS_CHILD | WS_VISIBLE, 20, 32, 35, 25, hwnd,
			(HMENU)ID_LABEL, NULL, NULL);

		hwndEditDiskSize = CreateWindowW(L"Edit", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			55, 55, 150, 20, hwnd, (HMENU)ID_EDIT,
			NULL, NULL);

		CreateWindowW(L"static", L"Size:",
			WS_CHILD | WS_VISIBLE, 20, 57, 35, 25, hwnd, 
			(HMENU)ID_LABEL, NULL, NULL);

		hwndEditDiskFolder = CreateWindowW(L"Edit", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_READONLY,
			55, 80, 150, 20, hwnd, (HMENU)ID_EDIT,
			NULL, NULL);

		CreateWindowW(L"static", L"Folder:",
			WS_CHILD | WS_VISIBLE, 20, 82, 35, 25, hwnd,
			(HMENU)ID_LABEL, NULL, NULL);

		hwndButtonBrowseFolders = CreateWindowW(L"button", L"Browse",
			WS_VISIBLE | WS_CHILD, 20, 105, 90, 25,
			hwnd, (HMENU)ID_BUTTON_OPEN_FOLDER, NULL, NULL);

		hwndButtonCreateAndMount = CreateWindowW(L"button", L"Create and mount",
			WS_VISIBLE | WS_CHILD, 115, 105, 90, 25,
			hwnd, (HMENU)ID_BUTTON_CREATE_AND_MOUNT, NULL, NULL);

		//Mount disk groupbox - left center

		CreateWindowW(L"Button", L"Mount disk",
			WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
			10, 150, 210, 90, hwnd, (HMENU)0, g_hinst, NULL);

		CreateWindowW(L"static", L"File:",
			WS_CHILD | WS_VISIBLE, 20, 172, 35, 25, hwnd,
			(HMENU)ID_LABEL, NULL, NULL);

		hwndEditChooseDisk = CreateWindowW(L"Edit", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_READONLY,
			55, 170, 150, 20, hwnd, (HMENU)ID_EDIT,
			NULL, NULL);

		hwndButtonOpenFile = CreateWindowW(L"button", L"Browse",
			WS_VISIBLE | WS_CHILD, 20, 195, 90, 25,
			hwnd, (HMENU)ID_BUTTON_OPEN_FILE, NULL, NULL);

		hwndButtonMount = CreateWindowW(L"button", L"Mount",
			WS_VISIBLE | WS_CHILD, 115, 195, 90, 25,
			hwnd, (HMENU)ID_BUTTON_MOUNT, NULL, NULL);

		//Choose disk groupbox - left bottom

		CreateWindowW(L"Button", L"Choose disk",
			WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
			10, 245, 210, 65, hwnd, (HMENU)0, g_hinst, NULL);

		CreateWindowW(L"static", L"Disk:",
			WS_CHILD | WS_VISIBLE, 20, 267, 35, 25, hwnd,
			(HMENU)ID_LABEL, NULL, NULL);

		hwndCombo = CreateWindowW(L"Combobox", NULL,
			WS_CHILD | WS_VISIBLE | CBS_DROPDOWN,
			55, 265, 50, 65, hwnd, NULL, g_hinst, NULL);

		hwndButtonChooseDisk = CreateWindowW(L"button", L"Show files",
			WS_VISIBLE | WS_CHILD, 115, 263, 90, 25,
			hwnd, (HMENU)ID_BUTTON_CHOOSE_DISK, NULL, NULL);
		
		AddItemsToCombobox(hwndCombo, driveLetters);

		//File treeview - center

		hwndTreeView = CreateATreeView(g_hinst, hwnd, 225, 16, 400, 250);
		/*AddItemToTreeView(hwndTreeView, L"l1", 1);
		AddItemToTreeView(hwndTreeView, L"l2", 2);
		AddItemToTreeView(hwndTreeView, L"l1", 1);
		AddItemToTreeView(hwndTreeView, L"l2", 2);*/
		addItemsToTreeView(driveLetters, hwndTreeView);

		static HTREEITEM item = FindItem(hwndTreeView, L"l1");
		//AddItemToParent(hwndTreeView, L"l3", item);
		
		static HWND hwndDebug = CreateWindowW(L"Edit", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
			400, 500, 150, 20, hwnd, (HMENU)ID_EDIT,
			NULL, NULL);

		EnumChildWindows(hwnd, (WNDENUMPROC)SetFont, (LPARAM)GetStockObject(DEFAULT_GUI_FONT));

		break;

	case WM_COMMAND:

		if (LOWORD(wParam) == ID_BUTTON_OPEN_FILE) {
			OpenFileDialog(hwndEditChooseDisk);
		}

		if (LOWORD(wParam) == ID_BUTTON_OPEN_FOLDER) {
			OpenFolderDialog(hwndEditDiskFolder);
		}

		if (LOWORD(wParam) == ID_BUTTON_CREATE_AND_MOUNT) {
			//create and mount
		}

		if (LOWORD(wParam) == ID_BUTTON_MOUNT) {
			//mount
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

			SetWindowText(hwndDebug, item.pszText);
		}
		return 0;
	}

	case WM_SETFOCUS:
		//SetWindowText(hwndDebug, L"Focus");
		break;

	case WM_KILLFOCUS:
		//SetWindowText(hwndDebug, L"Focus killed");
		break;

	case WM_DESTROY:

		PostQuitMessage(0);

		break;
	}

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void CenterWindow(HWND hwnd) {

	RECT rc = { 0 };

	GetWindowRect(hwnd, &rc);
	int win_w = rc.right - rc.left;
	int win_h = rc.bottom - rc.top;

	int screen_w = GetSystemMetrics(SM_CXSCREEN);
	int screen_h = GetSystemMetrics(SM_CYSCREEN);

	SetWindowPos(hwnd, HWND_TOP, (screen_w - win_w) / 2,
		(screen_h - win_h) / 2, 0, 0, SWP_NOSIZE);
}

void AddItemsToCombobox(HWND combobox, const std::vector<std::string>& items)
{
	for (size_t i = 0; i < items.size(); i++)
	{
		std::wstring wdl = std::wstring(items[i].begin(), items[i].end());
		SendMessage(combobox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)wdl.c_str());
	}
}

void OpenFolderDialog(HWND hwnd) {
	BROWSEINFO bi;
	wchar_t folderPath[MAX_PATH + 1];

	int iImage = 0;

	bi.hwndOwner = hwnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = NULL;
	bi.lpszTitle = TEXT("Choose folder");
	bi.ulFlags = BIF_NEWDIALOGSTYLE;
	bi.lpfn = NULL;
	bi.lParam = NULL;
	bi.iImage = iImage;

	auto pidl = SHBrowseForFolder(&bi);
	if (pidl) {
		SHGetPathFromIDList(pidl, folderPath);
		SetWindowText(hwnd, folderPath);
	}
}

void OpenFileDialog(HWND hwnd) {
	OPENFILENAME ofn;
	TCHAR szFile[MAX_PATH];

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.hwndOwner = hwnd;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = TEXT("All files(*.*)\0*.*\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrFileTitle = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn))
		SetWindowText(hwnd, ofn.lpstrFile);
}

bool CALLBACK SetFont(HWND child, LPARAM font) {
	SendMessage(child, WM_SETFONT, font, true);
	return true;
}