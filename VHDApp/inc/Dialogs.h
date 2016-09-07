#pragma once
#include <shtypes.h>
#include "resources.h"

PIDLIST_ABSOLUTE OpenFolderDialog(HWND hwnd);

void OpenFileDialog(HWND hwnd, OpenFileDialogAction action = ATTACH_DISK);

void ShowTimeDialog(HWND hwnd, std::chrono::steady_clock::time_point begin_time, std::chrono::steady_clock::time_point end_time, std::wstring wstrMessageBeginning);

