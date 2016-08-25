#pragma once
#include <Windows.h>
#include <rc6.h>

#include <vector>
#include <set>
#include <string>

using namespace std;

//Wyœwietlnie plików w folderze
bool GetFilesInDirectory(const char* path, std::vector<std::string>& files, std::vector<std::string>& dirs)
{
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(path)) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			if (ent->d_type == DT_DIR && ent->d_name[0] != '.' && ent->d_name[0] != '$')
				dirs.push_back(ent->d_name);
			else if (ent->d_type == DT_REG && ent->d_name[0] != '.' && ent->d_name[0] != '$')
				files.push_back(ent->d_name);
		}
		closedir(dir);
	}
	else {
		/* could not open directory */
		return EXIT_FAILURE;
	}
}

void Sort(const std::vector<std::string>& files, const std::string& diskPath, const std::string& folderToSort)
{
	std::string newFolder;
	std::set<std::string> extensions;

	for (int i = 0; i < files.size(); i++)
	{
		for (int j = files[i].length() - 1; j >= 0; j--)
		{
			if (files[i][j] == '.')
			{
				newFolder = diskPath + "\\" + files[i].substr(j + 1);
				if (extensions.insert(files[i].substr(j + 1)).second)
				{
					CreateDirectory(s2ws(newFolder).c_str(), NULL);
				}
				MoveFile(s2ws(folderToSort + "\\" + files[i]).c_str(), s2ws(newFolder + "\\" + files[i]).c_str());
			}
		}
	}
}

std::vector<string> GetDriveLetters()
{
	DWORD dwSize = MAX_PATH;
	wchar_t szLogicalDrives[MAX_PATH] = { 0 };
	DWORD dwResult = GetLogicalDriveStrings(dwSize, szLogicalDrives);
	std::vector<string> driveLetters;

	if (dwResult > 0 && dwResult <= MAX_PATH)
	{
		wchar_t* szSingleDrive = szLogicalDrives;
		while (*szSingleDrive)
		{
			//printf("Drive: %s\n", szSingleDrive);
			wstring wstr(szSingleDrive);
			driveLetters.push_back(string(wstr.begin(), wstr.end()));
			// get the next drive
			szSingleDrive += wcslen(szSingleDrive) + 1;
		}
	}

	return driveLetters;
}
