#include <windows.h>
#include <ShlObj.h>

#include "VHD.h"

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
#define ID_TREEVIEW 7

HINSTANCE g_hinst;
HANDLE hFont = CreateFont(20, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void CenterWindow(HWND hwnd);
void OpenFileDialog(HWND hwnd);
void OpenFolderDialog(HWND hwnd);
bool CALLBACK SetFont(HWND child, LPARAM font);
void AddItemsToCombobox(HWND combobox, const std::vector<std::string>& items);

HWND CreateATreeView(HWND hwndParent, int x = 0, int y = 0, int width = 100, int height = 100);
HTREEITEM AddItemToTreeView(HWND hwndTree, LPWSTR text, int nLevel);
HTREEITEM AddItemToParent(HWND hwndTree, LPWSTR text, HTREEITEM parent);
HTREEITEM FindItem(HWND hwndTV, const std::wstring& itemText);
HTREEITEM FindItemDepthFirstImpl(HWND hwndTV, HTREEITEM htStart, const std::wstring& itemText);
std::wstring GetItemText(HWND hwndTV, HTREEITEM htItem);

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
	std::vector<string> driveLetters = getDriveLetters(size);;

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

		hwndTreeView = CreateATreeView(hwnd, 225, 16, 400, 250);
		AddItemToTreeView(hwndTreeView, L"l1", 1);
		AddItemToTreeView(hwndTreeView, L"l2", 2);
		AddItemToTreeView(hwndTreeView, L"l1", 1);
		AddItemToTreeView(hwndTreeView, L"l2", 2);

		static HTREEITEM item = FindItem(hwndTreeView, L"l1");
		AddItemToParent(hwndTreeView, L"l3", item);
		
		static HWND hwndDebug = CreateWindowW(L"Edit", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
			400, 600, 150, 20, hwnd, (HMENU)ID_EDIT,
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

	case WM_SETFOCUS:
		SetWindowText(hwndDebug, L"Focus");
		break;

	case WM_KILLFOCUS:
		SetWindowText(hwndDebug, L"Focus killed");
		break;

	case WM_KEYDOWN:

		if (wParam == VK_ESCAPE) {

			int ret = MessageBoxW(hwnd, L"Are you sure to quit?",
				L"Message", MB_OKCANCEL);

			if (ret == IDOK) {
				SendMessage(hwnd, WM_CLOSE, 0, 0);
			}

		}

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

HWND CreateATreeView(HWND hwndParent, int x, int y, int width, int height)
{
	RECT rcClient;  // dimensions of client area 
	HWND hwndTV;    // handle to tree-view control 

					// Ensure that the common control DLL is loaded. 

					// Get the dimensions of the parent window's client area, and create 
					// the tree-view control. 
	GetClientRect(hwndParent, &rcClient);
	hwndTV = CreateWindowEx(0,
		WC_TREEVIEW,
		TEXT("Tree View"),
		WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASLINES,
		x,
		y,
		width,
		height,
		hwndParent,
		(HMENU)ID_TREEVIEW,
		g_hinst,
		NULL);

	return hwndTV;
}

HTREEITEM AddItemToParent(HWND hwndTree, LPWSTR text, HTREEITEM parent)
{
	TVITEM tvi;
	TVINSERTSTRUCT tvins;
	static HTREEITEM hPrev = (HTREEITEM)TVI_FIRST;
	static HTREEITEM hRootItem = NULL;
	//tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM;
	tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIS_STATEIMAGEMASK;
	//tvi.iImage = AddIconToTree(hwndTree, text);
	//tvi.iSelectedImage = tvi.iImage;
	tvi.pszText = text;
	tvi.cchTextMax = sizeof(tvi.pszText) / sizeof(tvi.pszText[0]);
	tvins.hInsertAfter = hPrev;
	tvins.item = tvi;
	tvins.hParent = parent;
	return (HTREEITEM)SendMessage(hwndTree, TVM_INSERTITEM, 0, (LPARAM)(LPTVINSERTSTRUCT)&tvins);
}

HTREEITEM FindItem(HWND hwndTV, const std::wstring& itemText)
{
	HTREEITEM htiRoot = TreeView_GetRoot(hwndTV);
	return FindItemDepthFirstImpl(hwndTV, htiRoot, itemText);
}

HTREEITEM FindItemDepthFirstImpl(HWND hwndTV, HTREEITEM htStart, const std::wstring& itemText)
{
	HTREEITEM htItemMatch = NULL;

	HTREEITEM htItemCurrent = htStart;
	// Iterate over items until there are no more items or we found a match
	while (htItemCurrent != NULL && htItemMatch == NULL)
	{
		if (GetItemText(hwndTV, htItemCurrent) == itemText)
		{
			htItemMatch = htItemCurrent;
		}
		else
		{
			// Traverse into child items
			htItemMatch = FindItemDepthFirstImpl(hwndTV, TreeView_GetChild(hwndTV, htItemCurrent), itemText);
		}
		htItemCurrent = TreeView_GetNextSibling(hwndTV, htItemCurrent);
	}

	return htItemMatch;
}

std::wstring GetItemText(HWND hwndTV, HTREEITEM htItem)
{
	static const size_t maxLen = 128;
	WCHAR buffer[maxLen + 1];

	TVITEMW tvi = { 0 };
	tvi.hItem = htItem;         // Treeview item to query
	tvi.mask = TVIF_TEXT;       // Request text only
	tvi.cchTextMax = maxLen;
	tvi.pszText = &buffer[0];
	if (TreeView_GetItem(hwndTV, &tvi))
	{
		return std::wstring(tvi.pszText);
	}
	else
	{
		return std::wstring();
	}
}

HTREEITEM AddItemToTreeView(HWND hwndTree, LPWSTR text, int nLevel)
{
	TVITEM tvi;
	TVINSERTSTRUCT tvins;
	static HTREEITEM hPrev = (HTREEITEM)TVI_FIRST;
	static HTREEITEM hRootItem = NULL;
	//tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM;
	tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIS_STATEIMAGEMASK;
	//tvi.iImage = AddIconToTree(hwndTree, text);
	//tvi.iSelectedImage = tvi.iImage;
	tvi.pszText = text;
	tvi.cchTextMax = sizeof(tvi.pszText) / sizeof(tvi.pszText[0]);
	tvins.hInsertAfter = hPrev;
	tvins.item = tvi;

	if (nLevel == 1)
	{
		tvins.hParent = TVI_ROOT;
	}
	else if (nLevel == 2)
	{
		tvins.hParent = hRootItem;
	}
	else
	{
		TVITEM tviSetup;
		tviSetup.hItem = hPrev;
		tviSetup.mask = TVIF_PARAM;
		TVITEM * tviLocal = &tviSetup;
		TreeView_GetItem(hwndTree, tviLocal);

		if (nLevel > tviLocal->lParam)
		{
			tvins.hParent = hPrev;
		}
		else
		{
			HTREEITEM hPrevLocal = TreeView_GetParent(hwndTree, hPrev);
			tviLocal->hItem = hPrevLocal;
			TreeView_GetItem(hwndTree, tviLocal);
			for (int i = nLevel; i <= tviLocal->lParam;)
			{
				HTREEITEM hPrevLocalTemp = TreeView_GetParent(hwndTree, hPrevLocal);
				hPrevLocal = hPrevLocalTemp;
				tviLocal->hItem = hPrevLocal;
				TreeView_GetItem(hwndTree, tviLocal);
			}
			tviLocal->mask = TVIF_TEXT;
			TreeView_GetItem(hwndTree, tviLocal);
			tvins.hParent = hPrevLocal;

		}
	}

	hPrev = (HTREEITEM)SendMessage(hwndTree, TVM_INSERTITEM, 0, (LPARAM)(LPTVINSERTSTRUCT)&tvins);

	if (hPrev == 0)
	{
		return false;
	}
	if (nLevel == 1)
	{
		hRootItem = hPrev;
	}

	return hPrev;
}

