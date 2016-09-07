#pragma once
#include <string>
#include <vector>

HWND CreateATreeView(HINSTANCE g_hinst, HWND hwndParent, int x = 0, int y = 0, int width = 100, int height = 100);
HTREEITEM AddItemToTreeView(HWND hwndTree, LPWSTR text, int nLevel);
HTREEITEM AddItemToParent(HWND hwndTree, LPWSTR text, HTREEITEM parent, int image = 1, int selectedImage = 1);
HTREEITEM FindItem(HWND hwndTV, const std::wstring& itemText);
HTREEITEM FindItemDepthFirstImpl(HWND hwndTV, HTREEITEM htStart, const std::wstring& itemText);
std::wstring GetItemText(HWND hwndTV, HTREEITEM htItem);
void AddItemsToTreeView(const std::vector<std::string>& items, HWND hwndTreeView, int level = 1);
std::string GetFullNodePath(HWND hwndTV, HTREEITEM hItem);
TVITEM GetSelectedNode(HWND hwndWindow, HWND hwndTV, LPNM_TREEVIEW& pntv, WCHAR* buffer);
int AddIconToTree(HWND hwndTree, const char* strPath);
void AddFilesAndDirsToTree(HWND hwndTV, HTREEITEM parent, const std::string& folderPath);
void SetImageList(HWND hwndTree);