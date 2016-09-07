#pragma once
#include <Windows.h>
#include <windowsx.h>
#include <vector>

namespace Combobox
{
	void AddItemsToCombobox(HWND combobox, const std::vector<std::string>& items);

	std::string TextFromComboboxToString(HWND combobox);
}