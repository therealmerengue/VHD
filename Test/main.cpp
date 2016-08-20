#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include "dirent.h"
#include "VHD.h"

using namespace std;

int main()
{
	std::vector<string> files;
	getFilesInDirectory("D:\\", files);
	cout << files.size() << endl;
	for each (string file in files)
	{
		cout << file << "\t" << endl;
	}
	return 0;
}