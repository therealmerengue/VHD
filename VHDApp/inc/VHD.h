#pragma once
#define DEFIND_GUID
#pragma comment(lib, "VirtDisk.lib")
#pragma comment( lib, "setupapi.lib" )

#define PHYS_PATH_LEN 1024+1
//GUID GUID_TEST = { 12345678 - 1234 - 5678 - 1234 - 000000000000 };
//GUID zGuid = GUID_NULL;

using namespace std;

#define START_ERROR_CHK()           \
    DWORD error = ERROR_SUCCESS;    \
    DWORD failedLine;               \
    string failedApi;

#define CHK( expr, api )            \
    if ( !( expr ) ) {              \
        error = GetLastError( );    \
        failedLine = __LINE__;      \
        failedApi = ( api );        \
        goto Error_Exit;            \
    }

#define END_ERROR_CHK()             \
    error = ERROR_SUCCESS;          \
    Error_Exit:                     \
    if ( ERROR_SUCCESS != error ) { \
        cout << failedApi << " failed at " << failedLine << " : Error Code - " << error << endl;    \
    }

namespace VHD
{
	int CountPhysicalDisks();

	void PrintErrorMessage(ULONG ErrorId);

	BOOL CreateVHD_Fixed(PCWSTR pszVhdPath, ULONG sizeInMB);

	BOOL OpenAndAttachVHD2(PCWSTR pszVhdPath, int diskIndex);
}
