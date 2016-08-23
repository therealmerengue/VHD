#pragma once
#include <Windows.h>
#include <string>

std::string toString(const std::wstring& wstr)
{
	return std::string(wstr.begin(), wstr.end());
}

std::wstring toWString(const std::string& str)
{
	return std::wstring(str.begin(), str.end());
}

std::wstring s2ws(const std::string& s)  // wstring ze stringa
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

std::string WindowTextToString(HWND hwndWindow)
{
	wchar_t diskNameBuffer[128];
	GetWindowText(hwndWindow, diskNameBuffer, 128);
	std::wstring wstrDiskName = std::wstring(diskNameBuffer);
	return toString(wstrDiskName);
}