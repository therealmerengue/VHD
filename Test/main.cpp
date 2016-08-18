#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

std::vector<string> getDriveLetters(int& size)
{
	DWORD dwSize = MAX_PATH;
	wchar_t szLogicalDrives[MAX_PATH] = { 0 };
	DWORD dwResult = GetLogicalDriveStrings(dwSize, szLogicalDrives);
	std::vector<string> driveLetters;
	size = 0;

	if (dwResult > 0 && dwResult <= MAX_PATH)
	{
		wchar_t* szSingleDrive = szLogicalDrives;
		while (*szSingleDrive)
		{
			size++;
			//printf("Drive: %s\n", szSingleDrive);
			wstring wstr(szSingleDrive);
			driveLetters.push_back(string(wstr.begin(), wstr.end()));
			// get the next drive
			szSingleDrive += wcslen(szSingleDrive) + 1;
		}
	}

	return driveLetters;
}

int main()
{
	int i = 0;
	vector<string> dls = getDriveLetters(i);
	for (size_t i = 0; i < dls.size(); i++)
	{
		cout << dls[i] << endl;
	}

	return 0;
}