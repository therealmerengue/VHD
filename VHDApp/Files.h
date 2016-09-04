#pragma once
#include <Windows.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <vector>
#include <set>
#include <string>
#include <sstream>
#include <fstream>

#include "Encryption.h"
#include "Dialogs.h"

using namespace std;



int IsRegularFile(const char *path)
{
	struct stat path_stat;
	stat(path, &path_stat);
	return S_ISREG(path_stat.st_mode);
}

int IsDirectory(const char *path) {
	struct stat statbuf;
	if (stat(path, &statbuf) != 0)
		return 0;
	return S_ISDIR(statbuf.st_mode);
}

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
				break;
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
			wstring wstr(szSingleDrive);
			driveLetters.push_back(string(wstr.begin(), wstr.end()));
			// get the next drive
			szSingleDrive += wcslen(szSingleDrive) + 1;
		}
	}

	return driveLetters;
}

void CreateEncryptedFile(const string& inputPath, const string& outputPath, const string& password)
{
	ifstream inputFile(inputPath);
	stringstream inputStream;

	inputStream << inputFile.rdbuf();
	inputFile.close();
	string inputText = inputStream.str();

	string outputText = Encrypt(password.c_str(), inputText.c_str());

	ofstream outputFile(outputPath);
	if (outputFile.is_open())
		outputFile << outputText;
	outputFile.close();
}

void EncryptFiles(const vector<string>& files, const string& inputFolderPath, const string& outputFolderPath, const string& password)
{
	for (int i = 0; i < files.size(); i++)
	{
		for (int j = files[i].length() - 1; j >= 0; j--)
		{
			if (files[i][j] == '.')
			{
				if (files[i].substr(j + 1) == "txt")
					CreateEncryptedFile(inputFolderPath + "\\" + files[i], outputFolderPath + "\\Crypt_" + files[i], password);
				
				break;
			}
		}
	}
}

void PerformSort(HWND hwnd, std::string folderToSort, std::string diskPath)
{
	vector<string> filesToSort;
	GetFilesInDirectory(folderToSort.c_str(), filesToSort, vector<string>());
	std::chrono::steady_clock::time_point begin_time = std::chrono::steady_clock::now();
	std::thread(Sort, filesToSort, diskPath, folderToSort).join(); //std::ref?
	std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
	ShowTimeDialog(hwnd, begin_time, end_time, L"Sorted in: ");
}
