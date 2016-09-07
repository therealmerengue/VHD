#include <Windows.h>

#include "Visual.h"

namespace Visual
{
	bool CALLBACK SetFont(HWND child, LPARAM font)
	{
		SendMessage(child, WM_SETFONT, font, true);
		return true;
	}

	void CenterWindow(HWND hwnd)
	{
		RECT rc = { 0 };

		GetWindowRect(hwnd, &rc);
		int win_w = rc.right - rc.left;
		int win_h = rc.bottom - rc.top;

		int screen_w = GetSystemMetrics(SM_CXSCREEN);
		int screen_h = GetSystemMetrics(SM_CYSCREEN);

		SetWindowPos(hwnd, HWND_TOP, (screen_w - win_w) / 2,
			(screen_h - win_h) / 2, 0, 0, SWP_NOSIZE);
	}
}