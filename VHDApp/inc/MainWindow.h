#pragma once
#include <Windows.h>

class MainWindow
{
private:
	HWND hWindow;
	WNDCLASSW windowClass;
	static std::unique_ptr<Treeview> tv;

	LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI HandleMsgStatic(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void AddMenus(HWND hwnd);
	
public:
	MainWindow(HINSTANCE hInstance);
	~MainWindow();

	void ProcessMessage();
};

