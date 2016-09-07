#pragma once
#include <string>

std::string toString(const std::wstring& wstr);

std::wstring toWString(const std::string& str);

std::wstring s2ws(const std::string& s);

std::string WindowTextToString(HWND hwndWindow);

bool CheckIfStringEmpty(std::string str, LPCWSTR errorMessage, HWND hwndMB, HWND hwndEdit);

bool CheckIfStringContainsNumbersOnly(std::string str, LPCWSTR errorMessage, HWND hwndMB, HWND hwndEdit);