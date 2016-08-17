#include <windows.h>
#include <ShlObj.h>

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define ID_EDIT_CHOOSE_DISK 1
#define ID_BUTTON_OPEN_FILE 2
#define ID_BUTTON_OPEN_FOLDER 3
#define ID_EDIT_DISK_NAME 4
#define ID_LABEL_DISK_NAME 5

HINSTANCE g_hinst;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void CenterWindow(HWND hwnd);
void OpenFileDialog(HWND hwnd);
void OpenFolderDialog(HWND hwnd);
bool CALLBACK SetFont(HWND child, LPARAM font);

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
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
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

	static HWND hwndEditChooseDisk, hwndCombo, hwndEditDiskName;
	HWND hwndButtonOpenFile, hwndButtonOpenFolder;

	switch (msg) {

	case WM_CREATE:

		CenterWindow(hwnd);

		CreateWindowW(L"Button", L"Choose disk file",
			WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
			10, 210, 320, 200, hwnd, (HMENU)0, g_hinst, NULL);

		hwndCombo = CreateWindowW(L"Combobox", NULL,
			WS_CHILD | WS_VISIBLE | CBS_DROPDOWN,
			150, 300, 120, 110, hwnd, NULL, g_hinst, NULL);

		hwndEditChooseDisk = CreateWindowW(L"Edit", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY,
			50, 250, 250, 20, hwnd, (HMENU)ID_EDIT_CHOOSE_DISK,
			NULL, NULL);

		hwndButtonOpenFile = CreateWindowW(L"button", L"Open file",
			WS_VISIBLE | WS_CHILD, 50, 300, 80, 25,
			hwnd, (HMENU)ID_BUTTON_OPEN_FILE, NULL, NULL);

		hwndButtonOpenFolder = CreateWindowW(L"button", L"Open folder",
			WS_VISIBLE | WS_CHILD, 50, 330, 80, 25,
			hwnd, (HMENU)ID_BUTTON_OPEN_FOLDER, NULL, NULL);

		CreateWindowW(L"Button", L"Create disk",
			WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
			10, 10, 320, 200, hwnd, (HMENU)0, g_hinst, NULL);

		hwndEditDiskName = CreateWindowW(L"Edit", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			110, 50, 150, 20, hwnd, (HMENU)ID_EDIT_DISK_NAME,
			NULL, NULL);

		CreateWindowW(L"static", L"Disk name:",
			WS_CHILD | WS_VISIBLE | WS_BORDER, 20, 50, 90, 20, hwnd, 
			(HMENU)ID_LABEL_DISK_NAME, NULL, NULL);

		EnumChildWindows(hwnd, (WNDENUMPROC)SetFont, (LPARAM)GetStockObject(DEFAULT_GUI_FONT));

		break;

	case WM_COMMAND:

		if (LOWORD(wParam) == ID_BUTTON_OPEN_FILE) {
			OpenFileDialog(hwndEditChooseDisk);
		}

		if (LOWORD(wParam) == ID_BUTTON_OPEN_FOLDER) {
			OpenFolderDialog(hwndEditChooseDisk);
		}

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

bool CALLBACK SetFont(HWND child, LPARAM font) {
	SendMessage(child, WM_SETFONT, font, true);
	return true;
}

void OpenFolderDialog(HWND hwnd) {
	BROWSEINFO bi;
	char folderPath[MAX_PATH + 1];

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
