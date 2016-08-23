#pragma once
#include <Windows.h>
#include <vector>

void AddItemsToCombobox(HWND combobox, const std::vector<std::string>& items)
{
	for (size_t i = 0; i < items.size(); i++)
	{
		std::wstring wdl = std::wstring(items[i].begin(), items[i].end());
		SendMessage(combobox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)wdl.c_str());
	}
}