#pragma once
#include <vector>

#define ID_TREEVIEW 7

HWND CreateATreeView(HINSTANCE g_hinst, HWND hwndParent, int x = 0, int y = 0, int width = 100, int height = 100);
HTREEITEM AddItemToTreeView(HWND hwndTree, LPWSTR text, int nLevel);
HTREEITEM AddItemToParent(HWND hwndTree, LPWSTR text, HTREEITEM parent);
HTREEITEM FindItem(HWND hwndTV, const std::wstring& itemText);
HTREEITEM FindItemDepthFirstImpl(HWND hwndTV, HTREEITEM htStart, const std::wstring& itemText);
std::wstring GetItemText(HWND hwndTV, HTREEITEM htItem);
void addItemsToTreeView(std::vector<string> items, HWND hwndTreeView, int level = 1);

HWND CreateATreeView(HINSTANCE g_hinst, HWND hwndParent, int x, int y, int width, int height)
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
	static HTREEITEM hRootItem = NULL;
	//tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM;
	tvi.mask = TVIF_TEXT | TVIF_IMAGE |
		TVIF_SELECTEDIMAGE | TVIF_PARAM;
	//tvi.iImage = AddIconToTree(hwndTree, text);
	//tvi.iSelectedImage = tvi.iImage;
	tvi.pszText = text;
	tvi.cchTextMax = sizeof(tvi.pszText) / sizeof(tvi.pszText[0]);
	tvi.lParam = 10;
	tvins.hInsertAfter = TVI_LAST;
	tvins.item = tvi;
	tvins.hParent = parent;
	return (HTREEITEM)SendMessage(hwndTree, TVM_INSERTITEM, 0, (LPARAM)(LPTVINSERTSTRUCT)&tvins);
	/*return TreeView_InsertItem(hwndTree, &tvins);*/
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
	//tvi.iImage = AddIconToTree(hwndTree, text);
	//tvi.iSelectedImage = tvi.iImage;
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

void addItemsToTreeView(std::vector<string> items, HWND hwndTreeView, int level)
{
	for (size_t i = 0; i < items.size(); i++)
	{
		std::wstring wdl = std::wstring(items[i].begin(), items[i].end());
		AddItemToTreeView(hwndTreeView, &wdl[0], level);
	}
}