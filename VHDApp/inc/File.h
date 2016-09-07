#pragma once
#include <vector>

using namespace std;

namespace File
{
	int IsRegularFile(const char *path);

	int IsDirectory(const char *path);

	bool GetFilesInDirectory(const char* path, std::vector<std::string>& files, std::vector<std::string>& dirs);

	void Sort(const std::vector<std::string>& files, const std::string& diskPath, const std::string& folderToSort);

	std::vector<string> GetDriveLetters();

	void CreateEncryptedFile(const string& inputPath, const string& outputPath, const string& password);

	void EncryptFiles(const vector<string>& files, const string& inputFolderPath, const string& outputFolderPath, const string& password);

	void PerformSort(HWND hwnd, std::string folderToSort, std::string diskPath);
}