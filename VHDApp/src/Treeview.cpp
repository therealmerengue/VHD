#pragma once
#include <Windows.h>
#include <CommCtrl.h>
#include <CommonControls.h>
#include <vector>

#include "StringOperations.h"
#include "resources.h"
#include "Files.h"
#include "Treeview.h"

Treeview::Treeview(HINSTANCE g_hinst, HWND hwndParent, int x, int y, int width, int height)
{
	this->hTV = CreateWindowEx(0,
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
}

HWND Treeview::GetHandle()
{
	return this->hTV;
}

HTREEITEM Treeview::AddItemToParent(LPWSTR text, HTREEITEM parent, int image, int selectedImage)
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
	return (HTREEITEM)SendMessage(this->hTV, TVM_INSERTITEM, 0, (LPARAM)(LPTVINSERTSTRUCT)&tvins);
}

HTREEITEM Treeview::FindItem(const std::wstring& itemText)
{
	HTREEITEM htiRoot = TreeView_GetRoot(this->hTV);
	return FindItemDepthFirstImpl(htiRoot, itemText);
}

HTREEITEM Treeview::FindItemDepthFirstImpl(HTREEITEM htStart, const std::wstring& itemText)
{
	HTREEITEM htItemMatch = NULL;

	HTREEITEM htItemCurrent = htStart;
	// Iterate over items until there are no more items or we found a match
	while (htItemCurrent != NULL && htItemMatch == NULL)
	{
		if (GetItemText(this->hTV, htItemCurrent) == itemText)
		{
			htItemMatch = htItemCurrent;
		}
		else
		{
			// Traverse into child items 
			htItemMatch = FindItemDepthFirstImpl(TreeView_GetChild(this->hTV, htItemCurrent), itemText);
		}
		htItemCurrent = TreeView_GetNextSibling(this->hTV, htItemCurrent);
	}

	return htItemMatch;
}

std::wstring Treeview::GetItemText(HWND hTV, HTREEITEM hItem)
{
	static const size_t maxLen = 128;
	WCHAR buffer[maxLen + 1];

	TVITEMW tvi = { 0 };
	tvi.hItem = hItem;         // Treeview item to query
	tvi.mask = TVIF_TEXT;       // Request text only
	tvi.cchTextMax = maxLen;
	tvi.pszText = &buffer[0];
	if (TreeView_GetItem(hTV, &tvi))
	{
		return std::wstring(tvi.pszText);
	}
	else
	{
		return std::wstring();
	}
}

HTREEITEM Treeview::AddItemToTreeView(LPWSTR text, int nLevel)
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
		TreeView_GetItem(this->hTV, tviLocal);

		if (nLevel > tviLocal->lParam)
		{
			tvins.hParent = hPrev;
		}
		else
		{
			HTREEITEM hPrevLocal = TreeView_GetParent(this->hTV, hPrev);
			tviLocal->hItem = hPrevLocal;
			TreeView_GetItem(this->hTV, tviLocal);
			for (int i = nLevel; i <= tviLocal->lParam;)
			{
				HTREEITEM hPrevLocalTemp = TreeView_GetParent(this->hTV, hPrevLocal);
				hPrevLocal = hPrevLocalTemp;
				tviLocal->hItem = hPrevLocal;
				TreeView_GetItem(this->hTV, tviLocal);
			}
			tviLocal->mask = TVIF_TEXT;
			TreeView_GetItem(this->hTV, tviLocal);
			tvins.hParent = hPrevLocal;

		}
	}

	hPrev = (HTREEITEM)SendMessage(this->hTV, TVM_INSERTITEM, 0, (LPARAM)(LPTVINSERTSTRUCT)&tvins);

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

void Treeview::AddItemsToTreeView(const std::vector<string>& items, int level)
{
	for (size_t i = 0; i < items.size(); i++)
	{
		std::wstring wdl = std::wstring(items[i].begin(), items[i].end());
		AddItemToTreeView(&wdl[0], level);
	}
}

TVITEM Treeview::GetSelectedNode(HWND hwndWindow, LPNM_TREEVIEW& pntv, WCHAR* buffer)
{
	TVITEM item;
	item.hItem = pntv->itemNew.hItem;
	item.mask = TVIF_TEXT;
	item.pszText = &buffer[0];// allocate buffer
	item.cchTextMax = 128;  // length of buffer
	SendMessage(hwndWindow, TVM_GETITEM, 0, (LPARAM)&item);
	TreeView_GetItem(this->hTV, &item);
	return item;
}

std::string Treeview::GetFullNodePath(HWND hTV, HTREEITEM hItem)
{
	std::string fullPath = "\\" + toString(GetItemText(hTV, hItem));
	HTREEITEM hItemParent = TreeView_GetParent(hTV, hItem);
	bool isVolume = true;

	while (hItemParent != NULL && hItemParent != TVI_ROOT)
	{
		fullPath.insert(0, "\\" + toString(GetItemText(hTV, hItemParent)));
		hItemParent = TreeView_GetParent(hTV, hItemParent);
		isVolume = false;
	}

	fullPath = fullPath.erase(0, 1); //slashes suck

	return isVolume ? fullPath : fullPath.erase(2, 1); //really bad
}

int Treeview::AddIconToTree(const char* strPath)
{
	SHFILEINFO sfi;
	memset(&sfi, 0, sizeof(sfi));
	string str = string(strPath);

	// SHGFI_SYSICONINDEX will return the icon's index within the shell image list
	SHGetFileInfo(&toWString(str)[0], FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi),
		SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES);
	return sfi.iIcon;
}

void Treeview::SetImageList()
{
	HIMAGELIST himg;
	if (SUCCEEDED(SHGetImageList(SHIL_SMALL, IID_IImageList, reinterpret_cast<void**>(&himg))))
		SendMessage(this->hTV, TVM_SETIMAGELIST, (WPARAM)TVSIL_NORMAL, (LPARAM)himg);
}

void Treeview::AddFilesAndDirsToTree(HTREEITEM parent, const string& folderPath)
{
	std::vector<string> files;
	std::vector<string> dirs;
	GetFilesInDirectory(folderPath.c_str(), files, dirs);

	for (size_t i = 0; i < files.size(); i++)
	{
		wstring wstr = s2ws(files[i]);
		this->AddItemToParent(&wstr[0], parent);
	}

	for (size_t i = 0; i < dirs.size(); i++)
	{
		wstring wstr = s2ws(dirs[i]);
		this->AddItemToParent(&wstr[0], parent, 3, 4);
	}
}