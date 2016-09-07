#include <windows.h>
#include <ShlObj.h>
#include <commoncontrols.h>
#include <string>
#include <thread>
#include <chrono>
#include <ctime>
#include <memory>

#include "VHD.h"
#include "Treeview.h"
#include "Files.h"
#include "Dialogs.h"
#include "Combobox.h"
#include "Visuals.h"
#include "StringOperations.h"
#include "resources.h"
#include "DialogBoxes.h"
#include "MainWindow.h"

using namespace std;

MainWindow::MainWindow(HINSTANCE hInstance)
{
	this->windowClass.style = CS_HREDRAW | CS_VREDRAW; // window is redrawn on move/resize
	this->windowClass.cbClsExtra = 0;
	this->windowClass.cbWndExtra = 0;
	this->windowClass.lpszClassName = L"Window";
	this->windowClass.hInstance = hInstance; // setting instance of the program
	this->windowClass.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	this->windowClass.lpszMenuName = NULL;
	this->windowClass.lpfnWndProc = &MainWindow::HandleMsgStatic;
	this->windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	this->windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	
	RegisterClassW(&this->windowClass);
	this->hWindow = CreateWindowW(this->windowClass.lpszClassName, L"VHD App",
		WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,
		100, 100, 585, 355, NULL, NULL, hInstance, NULL);

	ShowWindow(this->hWindow, SW_SHOWDEFAULT);
	UpdateWindow(this->hWindow);
}

MainWindow::~MainWindow()
{
}

LRESULT CALLBACK MainWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndTreeView;
	std::vector<string> driveLetters = GetDriveLetters();
	std::vector<string> files;
	std::vector<string> dirs;

	switch (msg)
	{
	case WM_CREATE:
	{
		CenterWindow(hwnd);
		AddMenus(hwnd);

		CreateWindowW(L"Button", L"VHD Setup",
			WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
			375, 15, 170, 125, hwnd, (HMENU)0, g_hinst, NULL);

		CreateWindowW(L"button", L"New disk",
			WS_VISIBLE | WS_CHILD, 385, 40, 150, 25,
			hwnd, (HMENU)ID_BUTTON_NEW_DISK, NULL, NULL);

		CreateWindowW(L"button", L"Mount disk",
			WS_VISIBLE | WS_CHILD, 385, 70, 150, 25,
			hwnd, (HMENU)ID_BUTTON_MOUNT_DISK, NULL, NULL);

		CreateWindowW(L"button", L"Choose disk",
			WS_VISIBLE | WS_CHILD, 385, 100, 150, 25,
			hwnd, (HMENU)ID_BUTTON_CHOOSE_DISK, NULL, NULL);

		CreateWindowW(L"Button", L"Select",
			WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
			375, 145, 170, 125, hwnd, (HMENU)0, g_hinst, NULL);

		CreateWindowW(L"button", L"Sort",
			WS_VISIBLE | WS_CHILD, 385, 170, 150, 25,
			hwnd, (HMENU)ID_BUTTON_SORT, NULL, NULL);

		CreateWindowW(L"button", L"Encrypt",
			WS_VISIBLE | WS_CHILD, 385, 200, 150, 25,
			hwnd, (HMENU)ID_BUTTON_ENCRYPT, NULL, NULL);

		CreateWindowW(L"button", L"Decrypt",
			WS_VISIBLE | WS_CHILD, 385, 230, 150, 25,
			hwnd, (HMENU)ID_BUTTON_DECRYPT, NULL, NULL);

		tv.reset(new Treeview(g_hinst, hwnd, 30, 20, 335, 250));

		tv->AddItemsToTreeView(driveLetters);
		tv->SetImageList();
		hwndTreeView = tv->GetHandle();

		EnumChildWindows(hwnd, (WNDENUMPROC)SetFont, (LPARAM)GetStockObject(DEFAULT_GUI_FONT));

		break;
	}

	case WM_COMMAND:
	{
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
				string folderToSort = tv->GetFullNodePath(hwndTreeView, selectedFolder);

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
				DialogBox(g_hinst, MAKEINTRESOURCE(IDD_NOFOLDERS), hwnd, (DLGPROC)NoFoldersDialogProc); //show choose disk dialog
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
				DialogBox(g_hinst, MAKEINTRESOURCE(IDD_NOFOLDERS), hwnd, (DLGPROC)NoFoldersDialogProc); //show choose disk dialog
			}
		}

		if (LOWORD(wParam == ID_BUTTON_ENCRYPT))
		{
			if (!g_diskPath.empty())
			{
				HTREEITEM selectedFile = TreeView_GetSelection(hwndTreeView);
				string fileToEncrypt = tv->GetFullNodePath(hwndTreeView, selectedFile);

				if (!selectedFile)
				{
					MessageBox(hwnd, L"Choose a file to encrypt from treeview and press Encrypt.", L"Error", MB_OK);
					break;
				}

				if (IsRegularFile(fileToEncrypt.c_str()) == 0)
				{
					MessageBox(hwnd, L"Choose a file, not a directory.", L"Error", MB_OK);
					break;
				}

				if (fileToEncrypt.find(".txt") != fileToEncrypt.size() - 4)
				{
					MessageBox(hwnd, L"Choose a txt file.", L"Error", MB_OK);
					break;
				}

				DialogBoxParam(g_hinst, MAKEINTRESOURCE(IDD_ENCRYPT), hwnd, (DLGPROC)EncryptDialogProc, (LPARAM)hwndTreeView); //encrypt
			}
			else
			{
				DialogBox(g_hinst, MAKEINTRESOURCE(IDD_NOFOLDERS), hwnd, (DLGPROC)NoFoldersDialogProc); //show choose disk dialog
			}

			break;
		}

		if (LOWORD(wParam == IDM_ENCRYPT))
		{
			if (!g_diskPath.empty())
			{
				DialogBoxParam(g_hinst, MAKEINTRESOURCE(IDD_ENCRYPT), hwnd, (DLGPROC)EncryptDialogProc, NULL); //encrypt
			}
			else
			{
				DialogBox(g_hinst, MAKEINTRESOURCE(IDD_NOFOLDERS), hwnd, (DLGPROC)NoFoldersDialogProc); //show choose disk dialog
			}

			break;
		}

		if (LOWORD(wParam == ID_BUTTON_DECRYPT))
		{
			if (!g_diskPath.empty())
			{
				HTREEITEM selectedFile = TreeView_GetSelection(hwndTreeView);
				string fileToDecrypt = tv->GetFullNodePath(hwndTreeView, selectedFile);

				if (!selectedFile)
				{
					MessageBox(hwnd, L"Choose a file to decrypt from treeview and press Decrypt.", L"Error", MB_OK);
					break;
				}

				if (IsRegularFile(fileToDecrypt.c_str()) == 0)
				{
					MessageBox(hwnd, L"Choose a file, not a directory.", L"Error", MB_OK);
					break;
				}

				if (fileToDecrypt.find(".txt") != fileToDecrypt.size() - 4)
				{
					MessageBox(hwnd, L"Choose a txt file.", L"Error", MB_OK);
					break;
				}

				DialogBoxParam(g_hinst, MAKEINTRESOURCE(IDD_DECRYPT), hwnd, (DLGPROC)DecryptDialogProc, (LPARAM)hwndTreeView); //encrypt
			}
			else
			{
				DialogBox(g_hinst, MAKEINTRESOURCE(IDD_NOFOLDERS), hwnd, (DLGPROC)NoFoldersDialogProc); //show choose disk dialog
			}

			break;
		}

		if (LOWORD(wParam == IDM_DECRYPT))
		{
			if (!g_diskPath.empty())
			{
				DialogBoxParam(g_hinst, MAKEINTRESOURCE(IDD_DECRYPT), hwnd, (DLGPROC)DecryptDialogProc, NULL); //encrypt
			}
			else
			{
				DialogBox(g_hinst, MAKEINTRESOURCE(IDD_NOFOLDERS), hwnd, (DLGPROC)NoFoldersDialogProc); //show choose disk dialog
			}

			break;
		}

		break;
	}
	case WM_NOTIFY:
	{
		LPNM_TREEVIEW pntv = (LPNM_TREEVIEW)lParam;

		if (pntv->hdr.code == TVN_SELCHANGED)
		{
			WCHAR buffer[128];
			TVITEM item = tv->GetSelectedNode(hwnd, pntv, buffer);
			std::string fullNodePath = tv->GetFullNodePath(hwndTreeView, item.hItem);

			if (!TreeView_GetChild(hwndTreeView, item.hItem))
			{
				tv->AddFilesAndDirsToTree(item.hItem, fullNodePath);
			}
		}

		break;
	}

	case WM_SETFOCUS:
	{
		EnableWindow(hwnd, TRUE);
		ShowWindow(hwnd, SW_RESTORE);

		TreeView_DeleteAllItems(hwndTreeView);

		if (tv)
		{
			tv->AddItemsToTreeView(GetDriveLetters());
			if (!g_diskPath.empty())
			{
				auto diskNode = tv->FindItem(s2ws(g_diskPath));
				tv->AddFilesAndDirsToTree(diskNode, g_diskPath);
				TreeView_Expand(hwndTreeView, diskNode, TVM_EXPAND);
			}
		}

		break;
	}
	case WM_CLOSE:
	{
		DestroyWindow(hwnd);
		break;
	}
	case WM_DESTROY:
	{
		DestroyWindow(hwnd);
		PostQuitMessage(0);
		break;
	}
	}

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void MainWindow::AddMenus(HWND hwnd)
{
	HMENU hMenubar;
	HMENU hMenuFile, hMenuAction;

	hMenubar = CreateMenu();
	hMenuFile = CreateMenu();
	hMenuAction = CreateMenu();

	AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hMenuFile, L"&VHD");

	AppendMenuW(hMenuFile, MF_STRING, IDM_DISK_NEW, L"&New disk");
	AppendMenuW(hMenuFile, MF_STRING, IDM_DISK_MOUNT, L"&Mount disk");
	AppendMenuW(hMenuFile, MF_STRING, IDM_DISK_CHOOSE, L"&Choose disk");
	AppendMenuW(hMenuFile, MF_SEPARATOR, 0, NULL);
	AppendMenuW(hMenuFile, MF_STRING, IDM_QUIT, L"&Quit");

	AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hMenuAction, L"&Folders");
	AppendMenuW(hMenuAction, MF_STRING, IDM_SORT, L"&Sort");
	AppendMenuW(hMenuAction, MF_STRING, IDM_ENCRYPT, L"&Encrypt");
	AppendMenuW(hMenuAction, MF_STRING, IDM_DECRYPT, L"&Decrypt");

	SetMenu(hwnd, hMenubar);
}

LRESULT WINAPI MainWindow::HandleMsgStatic(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// retrieve ptr to window class
	MainWindow* const pWnd = reinterpret_cast<MainWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	// forward message to window class handler
	return pWnd->WndProc(hWnd, msg, wParam, lParam);
}

void MainWindow::ProcessMessage()
{
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

std::unique_ptr<Treeview> MainWindow::tv(nullptr);