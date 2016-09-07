#include <windows.h>
#include <ShlObj.h>
#include <commoncontrols.h>
#include <string>
#include <thread>
#include <chrono>
#include <ctime>
#include <memory>

#include "VHD.h"
#include "Treeview.h"
#include "File.h"
#include "Dialog.h"
#include "Combobox.h"
#include "Visual.h"
#include "StringOperations.h"
#include "resources.h"
#include "DialogProc.h"

#include "MainWindow.h"

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "comctl32.lib")

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) 
{	
	MainWindow mw(hInstance);
	mw.ProcessMessage();

	return 0;
}