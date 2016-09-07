#pragma once
#include <string>
#include <vector>

class Treeview
{
private:
	HWND hTV;
	
public:
	Treeview(HINSTANCE g_hinst, HWND hwndParent, int x, int y, int width, int height);
	HWND GetHandle();
	
	int AddIconToTree(const char* strPath);
	HTREEITEM AddItemToTreeView(LPWSTR text, int nLevel);
	HTREEITEM AddItemToParent(LPWSTR text, HTREEITEM parent, int image = 1, int selectedImage = 1);
	void AddItemsToTreeView(const std::vector<std::string>& items, int level = 1);
	void AddFilesAndDirsToTree(HTREEITEM parent, const std::string& folderPath);
		
	HTREEITEM FindItem(const std::wstring& itemText);
	HTREEITEM FindItemDepthFirstImpl(HTREEITEM htStart, const std::wstring& itemText);
	
	static std::wstring GetItemText(HWND hTV, HTREEITEM htItem);	
	static std::string GetFullNodePath(HWND hTV, HTREEITEM hItem);
	TVITEM GetSelectedNode(HWND hwndWindow, LPNM_TREEVIEW& pntv, WCHAR* buffer);
	
	void SetImageList();
};