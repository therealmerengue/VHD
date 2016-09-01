#pragma once
#include <vector>
#include <Windows.h>
#include "StringOperations.h"
#include "resources.h"
#include "Files.h"

HWND CreateATreeView(HINSTANCE g_hinst, HWND hwndParent, int x = 0, int y = 0, int width = 100, int height = 100);
HTREEITEM AddItemToTreeView(HWND hwndTree, LPWSTR text, int nLevel);
HTREEITEM AddItemToParent(HWND hwndTree, LPWSTR text, HTREEITEM parent, int image = 1, int selectedImage = 1);
HTREEITEM FindItem(HWND hwndTV, const std::wstring& itemText);
HTREEITEM FindItemDepthFirstImpl(HWND hwndTV, HTREEITEM htStart, const std::wstring& itemText);
std::wstring GetItemText(HWND hwndTV, HTREEITEM htItem);
void AddItemsToTreeView(const std::vector<string>& items, HWND hwndTreeView, int level = 1);
std::string GetFullNodePath(HWND hwndTV, HTREEITEM hItem);
TVITEM GetSelectedNode(HWND hwndWindow, HWND hwndTV, LPNM_TREEVIEW& pntv, WCHAR* buffer);
int AddIconToTree(HWND hwndTree, const char* strPath);
void AddFilesAndDirsToTree(HWND hwndTV, HTREEITEM parent, const string& folderPath);

HWND CreateATreeView(HINSTANCE g_hinst, HWND hwndParent, int x, int y, int width, int height)
{
	HWND hwndTV;    // handle to tree-view control 

	hwndTV = CreateWindowEx(0,
		WC_TREEVIEW,
		TEXT("Tree View"),
		WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASLINES | WS_HSCROLL | WS_VSCROLL,
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

HTREEITEM AddItemToParent(HWND hwndTree, LPWSTR text, HTREEITEM parent, int image, int selectedImage)
{
	TVITEM tvi;
	TVINSERTSTRUCT tvins;
	static HTREEITEM hRootItem = NULL;
	tvi.mask = TVIF_TEXT | TVIF_IMAGE |
		TVIF_SELECTEDIMAGE | TVIF_PARAM;
	tvi.iImage = image;
	tvi.iSelectedImage = selectedImage;
	tvi.pszText = text;
	tvi.cchTextMax = sizeof(tvi.pszText) / sizeof(tvi.pszText[0]);
	tvi.lParam = 10;
	tvins.hInsertAfter = TVI_LAST;
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
	tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIS_STATEIMAGEMASK | TVIF_PARAM;
	tvi.iImage = 7;
	tvi.iSelectedImage = tvi.iImage;
	tvi.pszText = text;
	tvi.cchTextMax = sizeof(tvi.pszText) / sizeof(tvi.pszText[0]);
	tvi.lParam = 10;
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

void AddItemsToTreeView(const std::vector<string>& items, HWND hwndTreeView, int level)
{
	for (size_t i = 0; i < items.size(); i++)
	{
		std::wstring wdl = std::wstring(items[i].begin(), items[i].end());
		AddItemToTreeView(hwndTreeView, &wdl[0], level);
	}
}

TVITEM GetSelectedNode(HWND hwndWindow, HWND hwndTV, LPNM_TREEVIEW& pntv, WCHAR* buffer)
{
	TVITEM item;
	item.hItem = pntv->itemNew.hItem;
	item.mask = TVIF_TEXT;
	item.pszText = &buffer[0];// allocate buffer
	item.cchTextMax = 128;  // length of buffer
	SendMessage(hwndWindow, TVM_GETITEM, 0, (LPARAM)&item);
	TreeView_GetItem(hwndTV, &item);
	return item;
}

std::string GetFullNodePath(HWND hwndTV, HTREEITEM hItem)
{
	std::string fullPath = "\\" + toString(GetItemText(hwndTV, hItem));
	HTREEITEM hItemParent = TreeView_GetParent(hwndTV, hItem);
	bool isVolume = true;

	while (hItemParent != NULL && hItemParent != TVI_ROOT)
	{
		fullPath.insert(0, "\\" + toString(GetItemText(hwndTV, hItemParent)));
		hItemParent = TreeView_GetParent(hwndTV, hItemParent);
		isVolume = false;
	}

	fullPath = fullPath.erase(0, 1); //slashes suck

	return isVolume ? fullPath : fullPath.erase(2, 1); //really bad
}

int AddIconToTree(HWND hwndTree, const char* strPath)
{
	SHFILEINFO sfi;
	memset(&sfi, 0, sizeof(sfi));
	string str = string(strPath);

	// SHGFI_SYSICONINDEX will return the icon's index within the shell image list
	SHGetFileInfo(&toWString(str)[0], FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi),
		SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES);
	return sfi.iIcon;
}

void SetImageList(HWND hwndTree)
{
	HIMAGELIST himg;
	if (SUCCEEDED(SHGetImageList(SHIL_SMALL, IID_IImageList, reinterpret_cast<void**>(&himg))))
		SendMessage(hwndTree, TVM_SETIMAGELIST, (WPARAM)TVSIL_NORMAL, (LPARAM)himg);
}

void AddFilesAndDirsToTree(HWND hwndTV, HTREEITEM parent, const string& folderPath)
{
	std::vector<string> files;
	std::vector<string> dirs;
	GetFilesInDirectory(folderPath.c_str(), files, dirs);

	for (size_t i = 0; i < files.size(); i++)
	{
		wstring wstr = s2ws(files[i]);
		AddItemToParent(hwndTV, &wstr[0], parent);
	}

	for (size_t i = 0; i < dirs.size(); i++)
	{
		wstring wstr = s2ws(dirs[i]);
		AddItemToParent(hwndTV, &wstr[0], parent, 3, 4);
	}
}