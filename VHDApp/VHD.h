#include <windows.h>
#include <stdio.h>
#include <winioctl.h> //magic trick :D
#include <initguid.h>
#include <virtdisk.h>
#include <commctrl.h>
#include <comdef.h>

#include <set>
#include <string>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <Windows.h>
#include <Setupapi.h>
#include <Ntddstor.h>
#include "dirent.h"

#define DEFIND_GUID
#pragma comment(lib, "VirtDisk.lib")
#pragma comment( lib, "setupapi.lib" )

#define PHYS_PATH_LEN 1024+1
GUID GUID_TEST = { 12345678 - 1234 - 5678 - 1234 - 000000000000 };
GUID zGuid = GUID_NULL;

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

int CountPhysicalDisks() //zwraca liczbę dysków fizycznych (jednocześnie pierwszy "dostępny" indeks dla nowego dysku)
{
	HDEVINFO diskClassDevices;
	GUID diskClassDeviceInterfaceGuid = GUID_DEVINTERFACE_DISK;
	SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
	PSP_DEVICE_INTERFACE_DETAIL_DATA deviceInterfaceDetailData;
	DWORD requiredSize;
	DWORD deviceIndex;
	int firstFreeIndex = 0;

	HANDLE disk = INVALID_HANDLE_VALUE;
	STORAGE_DEVICE_NUMBER diskNumber;
	DWORD bytesReturned;

	START_ERROR_CHK();

	//
	// Get the handle to the device information set for installed
	// disk class devices. Returns only devices that are currently
	// present in the system and have an enabled disk device
	// interface.
	//
	diskClassDevices = SetupDiGetClassDevs(&diskClassDeviceInterfaceGuid,
		NULL,
		NULL,
		DIGCF_PRESENT |
		DIGCF_DEVICEINTERFACE);
	CHK(INVALID_HANDLE_VALUE != diskClassDevices,
		"SetupDiGetClassDevs");

	ZeroMemory(&deviceInterfaceData, sizeof(SP_DEVICE_INTERFACE_DATA));
	deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	deviceIndex = 0;

	while (SetupDiEnumDeviceInterfaces(diskClassDevices,
		NULL,
		&diskClassDeviceInterfaceGuid,
		deviceIndex,
		&deviceInterfaceData)) {

		++deviceIndex;

		SetupDiGetDeviceInterfaceDetail(diskClassDevices,
			&deviceInterfaceData,
			NULL,
			0,
			&requiredSize,
			NULL);
		CHK(ERROR_INSUFFICIENT_BUFFER == GetLastError(),
			"SetupDiGetDeviceInterfaceDetail - 1");

		deviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(requiredSize);
		CHK(NULL != deviceInterfaceDetailData,
			"malloc");

		ZeroMemory(deviceInterfaceDetailData, requiredSize);
		deviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

		CHK(SetupDiGetDeviceInterfaceDetail(diskClassDevices,
			&deviceInterfaceData,
			deviceInterfaceDetailData,
			requiredSize,
			NULL,
			NULL),
			"SetupDiGetDeviceInterfaceDetail - 2");

		disk = CreateFile(deviceInterfaceDetailData->DevicePath,
			GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		CHK(INVALID_HANDLE_VALUE != disk,
			"CreateFile");

		CHK(DeviceIoControl(disk,
			IOCTL_STORAGE_GET_DEVICE_NUMBER,
			NULL,
			0,
			&diskNumber,
			sizeof(STORAGE_DEVICE_NUMBER),
			&bytesReturned,
			NULL),
			"IOCTL_STORAGE_GET_DEVICE_NUMBER");

		CloseHandle(disk);
		disk = INVALID_HANDLE_VALUE;

		/*cout << deviceInterfaceDetailData->DevicePath << endl;
		cout << "\\\\?\\PhysicalDrive" << diskNumber.DeviceNumber << endl;
		cout << endl;*/
		firstFreeIndex++;
	}
	CHK(ERROR_NO_MORE_ITEMS == GetLastError(),
		"SetupDiEnumDeviceInterfaces");

	END_ERROR_CHK();


	if (INVALID_HANDLE_VALUE != diskClassDevices) {
		SetupDiDestroyDeviceInfoList(diskClassDevices);
	}

	if (INVALID_HANDLE_VALUE != disk) {
		CloseHandle(disk);
	}

	return firstFreeIndex;
}

std::wstring s2ws(const std::string& s)  // wstring ze stringa
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

std::string toString(const std::wstring& wstr)
{
	return std::string(wstr.begin(), wstr.end());
}

std::wstring toWString(const std::string& str)
{
	return std::wstring(str.begin(), str.end());
}

void PrintErrorMessage(ULONG ErrorId)
{
	PVOID Message = NULL;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		ErrorId,
		0,
		(LPWSTR)&Message,
		16,
		NULL);

	wprintf(L"%s\n", Message);
	LocalFree(Message);
}

void usage()
{
	printf("CppVhdAPI.exe -[cxaomdgpe] -f <vhdfile> -s <size>\n");
	printf("-c CreateVirtualDisk............input: -f <vhd file name>, -s <size in MB>\n");
	printf("-a AttachVirtualDisk............input: -f <vhd file name>\n");
	printf("-d DetachVirtualDisk............input: -f <vhd file name>\n");
	//printf("-g GetVirtualDiskInformation....input: -f <vhd file name>\n");
	printf("-p GetVirtualDiskPhysicalPath...input: -f <vhd file name> -- note: must be attached\n");
	// printf("-e SetVirtualDiskInformation....input: -f <vhd file name>, -u <new GUID>\n");
	printf("Examples:\n");
	printf("  Create a 3.6 Gb VHD named 'mytest.vhd'\n");
	printf("CppVhdAPI.exe -c -f c:\\testdir\\mytest.vhd -s 3600\n\n");
	printf("  Attach a VHD named 'mytest.vhd'\n");
	printf("CppVhdAPI.exe -a -f c:\\testdir\\mytest.vhd\n\n");
	//printf("  Set VHD GUID 'mytest.vhd'\n");
	//printf("CppVhdAPI.exe -e -f c:\\testdir\\mytest.vhd -u {12345678-1234-5678-1234-000000000000}\n");
}

BOOL CreateVHD_Fixed(PCWSTR pszVhdPath, ULONG sizeInMB)
{
	BOOL bRet = FALSE;
	HANDLE hvhd;
	CREATE_VIRTUAL_DISK_PARAMETERS  params;
	VIRTUAL_DISK_ACCESS_MASK        mask;
	VIRTUAL_STORAGE_TYPE            vst =
	{
		VIRTUAL_STORAGE_TYPE_DEVICE_VHD,
		VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT
	};

	wprintf(L"CreateVHD_Fixed %s, size (MB) %d\n", pszVhdPath, sizeInMB);

	params.Version1.UniqueId = GUID_NULL;
	params.Version1.BlockSizeInBytes = 0;
	params.Version1.MaximumSize = sizeInMB * 1024 * 1024;
	params.Version1.ParentPath = NULL;
	params.Version1.SourcePath = NULL;
	params.Version1.SectorSizeInBytes = 512;
	params.Version = CREATE_VIRTUAL_DISK_VERSION_1;
	mask = VIRTUAL_DISK_ACCESS_CREATE;

	DWORD ret = CreateVirtualDisk(&vst,
		pszVhdPath,
		mask,
		NULL,
		// To create a dynamic disk, use CREATE_VIRTUAL_DISK_FLAG_NONE instead.
		CREATE_VIRTUAL_DISK_FLAG_FULL_PHYSICAL_ALLOCATION,
		0,
		&params,
		NULL,
		&hvhd);

	if (ret == ERROR_SUCCESS)
	{
		bRet = TRUE;
	}
	else
	{
		bRet = FALSE;
		printf("failed to create vdisk...err 0x%x\n", ret);
		PrintErrorMessage(GetLastError());
	}

	if (INVALID_HANDLE_VALUE != hvhd)
	{
		CloseHandle(hvhd);
	}

	return bRet;
}

BOOL OpenAndAttachVHD2(PCWSTR pszVhdPath, int diskIndex)
{
	BOOL bRet = FALSE;
	HANDLE hVhd = INVALID_HANDLE_VALUE;
	DWORD ret;
	OPEN_VIRTUAL_DISK_PARAMETERS oparams;
	ATTACH_VIRTUAL_DISK_PARAMETERS iparams;
	VIRTUAL_STORAGE_TYPE            vst =
	{
		VIRTUAL_STORAGE_TYPE_DEVICE_VHD,
		VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT
	};

	wprintf(L"OpenAndAttachVHD %s\n", pszVhdPath);

	oparams.Version = OPEN_VIRTUAL_DISK_VERSION_1;
	oparams.Version1.RWDepth = OPEN_VIRTUAL_DISK_RW_DEPTH_DEFAULT;

	iparams.Version = ATTACH_VIRTUAL_DISK_VERSION_1;

	ret = OpenVirtualDisk(&vst, pszVhdPath,
		VIRTUAL_DISK_ACCESS_ATTACH_RW | VIRTUAL_DISK_ACCESS_GET_INFO | VIRTUAL_DISK_ACCESS_DETACH,
		OPEN_VIRTUAL_DISK_FLAG_NONE, &oparams, &hVhd);

	if (ERROR_SUCCESS == ret)
	{
		printf("success opening vdisk...\n");
		ret = AttachVirtualDisk(hVhd, NULL, //ATTACH_VIRTUAL_DISK_FLAG_NO_DRIVE_LETTER - dodana flaga
			ATTACH_VIRTUAL_DISK_FLAG_PERMANENT_LIFETIME, 0, &iparams, NULL);

		if (ERROR_SUCCESS == ret)
		{
			printf("success attaching vdisk...\n");
		}
		else
		{
			printf("failed to attach vdisk...err 0x%x\n", ret);
			PrintErrorMessage(GetLastError());
			bRet = FALSE;
		}


		/////////////////////////////////////////
		/////////////////////////////////////////
		/////////////////////////////////////////

		std::wstring MountPointTemp = s2ws("Q:\\");
		std::wstring VolumeNameTemp = s2ws("\\?\PHYSICALDRIVE1 vhdname");

		BOOL bResult = FALSE;     // generic results flag   
		HANDLE hDevice;               // handle to the drive to be examined  
		DWORD junk = 0;                   // discard results 

										  //Tu jest zmienione:
		string sDiskIndex = std::to_string(diskIndex);
		string deviceName = "\\\\.\\PhysicalDrive" + sDiskIndex;
		std::wstring stemp = s2ws(deviceName);
		LPCWSTR dName = stemp.c_str();

		hDevice = CreateFile(dName, //"\\\\.\\PhysicalDrive1",  // drive to open 
			GENERIC_READ | GENERIC_WRITE,                // no access to the drive 
			FILE_SHARE_READ | // share mode 
			FILE_SHARE_WRITE,
			NULL,             // default security attributes 
			OPEN_EXISTING,    // disposition 
			0,                // file attributes 
			NULL);            // do not copy file attributes 
		if (hDevice == INVALID_HANDLE_VALUE) // cannot open the drive 
		{
			std::cout << "Cos nie dziala w hDevice \n";
		}

		cout << SetVolumeMountPoint(L"H:\\", dName) << endl;
		cout << GetLastError() << endl;

		CREATE_DISK dsk;
		dsk.PartitionStyle = PARTITION_STYLE_MBR;
		dsk.Mbr.Signature = 9999;

		// Initialize disk
		bResult = DeviceIoControl(hDevice,        // device to be queried 
			IOCTL_DISK_CREATE_DISK,  // operation to perform 
			&dsk, sizeof(dsk),        //sizeof(pdg),     // output buffer 
			NULL, 0,                // no output buffer 
			&junk,                    // # bytes returned 
			NULL);
		if (!bResult)
		{
			std::cout << "Cos nie dziala w DeviceIOontrol() \n";
		}

		bResult = DeviceIoControl(hDevice,
			IOCTL_DISK_UPDATE_PROPERTIES,
			NULL, 0, NULL, 0, &junk, NULL);
		if (!bResult)
		{
			return GetLastError();
		}

		LARGE_INTEGER lgPartitionSize;
		lgPartitionSize.QuadPart = (1024 * 1024 * 1024);
		DWORD dwDriverLayoutInfoExLen = sizeof(DRIVE_LAYOUT_INFORMATION_EX) + 3 * sizeof(PARTITION_INFORMATION_EX);
		DRIVE_LAYOUT_INFORMATION_EX *pdg = (DRIVE_LAYOUT_INFORMATION_EX *)new BYTE[dwDriverLayoutInfoExLen];
		if (pdg == NULL)
		{
			return -1;
		}
		SecureZeroMemory(pdg, dwDriverLayoutInfoExLen);
		// set RewritePartition=true in every partition to force rewrite.   
		// 	for (int item = 0; item < 4; item++){
		// 		pdg->PartitionEntry[item].RewritePartition = 1;
		// 		pdg->PartitionEntry[item].PartitionNumber = 0;
		// 	}

		pdg->PartitionStyle = PARTITION_STYLE_MBR;
		pdg->PartitionCount = 1;
		pdg->Mbr.Signature = 99999;

		pdg->PartitionEntry[0].PartitionStyle = PARTITION_STYLE_MBR;
		pdg->PartitionEntry[0].StartingOffset.QuadPart = 32256;   // 63扇区
		pdg->PartitionEntry[0].PartitionLength.QuadPart = lgPartitionSize.QuadPart * 36;
		pdg->PartitionEntry[0].PartitionNumber = 1;
		pdg->PartitionEntry[0].RewritePartition = TRUE;
		pdg->PartitionEntry[0].Mbr.PartitionType = PARTITION_NTFT; // PARTITION_IFS (NTFS partition or logical drive)   
		pdg->PartitionEntry[0].Mbr.BootIndicator = TRUE;
		pdg->PartitionEntry[0].Mbr.RecognizedPartition = 1;
		pdg->PartitionEntry[0].Mbr.HiddenSectors = 32256 / 512;

		// partition a disk
		bResult = DeviceIoControl(hDevice,        // device to be queried 
			IOCTL_DISK_SET_DRIVE_LAYOUT_EX,  // operation to perform 
			pdg, sizeof DRIVE_LAYOUT_INFORMATION_EX,        //sizeof(pdg),     // output buffer 
			NULL, 0,                // no output buffer 
			&junk,                    // # bytes returned 
			NULL);
		if (!bResult)
		{
			return GetLastError();
		}
		/*
		bResult = DeviceIoControl(hDevice,
		IOCTL_DISK_UPDATE_PROPERTIES,
		NULL, 0, NULL, 0, &junk, NULL);
		if (!bResult)
		{
		return GetLastError();
		}
		
		PARTITION_INFORMATION_EX dskinfo;
		PARTITION_INFORMATION_MBR mbrinfo;
		mbrinfo.PartitionType = PARTITION_NTFT;
		mbrinfo.HiddenSectors = (32256 / 512);
		mbrinfo.BootIndicator = 1;
		mbrinfo.RecognizedPartition = 1;

		dskinfo.PartitionStyle = PARTITION_STYLE_MBR;
		dskinfo.StartingOffset.QuadPart = 32256;
		dskinfo.PartitionLength.QuadPart = lgPartitionSize.QuadPart * 36;
		dskinfo.PartitionNumber = 1;
		dskinfo.RewritePartition = TRUE;
		dskinfo.Mbr = mbrinfo;*/

		// IOCTL_DISK_SET_PARTITION_INFO_EX can not perform successful
		/*bResult = DeviceIoControl(hDevice,        // device to be queried
		IOCTL_DISK_SET_PARTITION_INFO_EX,  // operation to perform
		&dskinfo, sizeof(dskinfo),        //sizeof(pdg),     // output buffer
		NULL, 0,                // no output buffer
		&junk,                    // # bytes returned
		//(LPOVERLAPPED)
		NULL);
		if (!bResult)
		{
		return GetLastError();
		}*/
	}
	else
	{
		printf("failed to open vdisk...err 0x%x\n", ret);
		PrintErrorMessage(GetLastError());
		bRet = FALSE;
	}

	if (INVALID_HANDLE_VALUE != hVhd)
	{
		CloseHandle(hVhd);
	}

	return bRet;
}

/*typedef struct _STORAGE_DEVICE_NUMBER {
DEVICE_TYPE  DeviceType;
ULONG  DeviceNumber;
ULONG  PartitionNumber;
} STORAGE_DEVICE_NUMBER, *PSTORAGE_DEVICE_NUMBER;*/

BOOL OpenAndAttachVHD(PCWSTR pszVhdPath)
{
	BOOL bRet = FALSE;
	HANDLE hVhd = INVALID_HANDLE_VALUE;
	DWORD ret;
	OPEN_VIRTUAL_DISK_PARAMETERS oparams;
	ATTACH_VIRTUAL_DISK_PARAMETERS iparams;
	VIRTUAL_STORAGE_TYPE            vst =
	{
		VIRTUAL_STORAGE_TYPE_DEVICE_VHD,
		VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT
	};

	wprintf(L"OpenAndAttachVHD %s\n", pszVhdPath);

	oparams.Version = OPEN_VIRTUAL_DISK_VERSION_1;
	oparams.Version1.RWDepth = OPEN_VIRTUAL_DISK_RW_DEPTH_DEFAULT;

	iparams.Version = ATTACH_VIRTUAL_DISK_VERSION_1;

	ret = OpenVirtualDisk(&vst, pszVhdPath,
		VIRTUAL_DISK_ACCESS_ATTACH_RW | VIRTUAL_DISK_ACCESS_GET_INFO | VIRTUAL_DISK_ACCESS_DETACH,
		OPEN_VIRTUAL_DISK_FLAG_NONE, &oparams, &hVhd);

	if (ERROR_SUCCESS == ret)
	{
		printf("success opening vdisk...\n");
		ret = AttachVirtualDisk(hVhd, NULL,
			ATTACH_VIRTUAL_DISK_FLAG_PERMANENT_LIFETIME, 0, &iparams, NULL);
		if (ERROR_SUCCESS == ret)
		{
			printf("success attaching vdisk...\n");
		}
		else
		{
			printf("failed to attach vdisk...err 0x%x\n", ret);
			PrintErrorMessage(GetLastError());
			bRet = FALSE;
		}

		/////////////////////////////////////////////
		//// TUTAJ COS PROBUJE ROBIC
		/////////////////////////////////////////////

		std::wstring MountPointTemp = s2ws("Q:");
		std::wstring VolumeNameTemp = s2ws("\\?\PHYSICALDRIVE1 vhdname");

#ifdef UNICODE
		LPCWSTR MountPoint = MountPointTemp.c_str();
		LPCWSTR VolumeName = VolumeNameTemp.c_str();
#else
		LPCWSTR result = s.c_str();
#endif

		SetVolumeMountPoint(MountPoint, VolumeName);

		/////////////////////////////////////////////
		////
		/////////////////////////////////////////////
	}
	else
	{
		printf("failed to open vdisk...err 0x%x\n", ret);
		PrintErrorMessage(GetLastError());
		bRet = FALSE;
	}

	if (INVALID_HANDLE_VALUE != hVhd)
	{
		CloseHandle(hVhd);
	}

	return bRet;
}

BOOL OpenAndDetachVHD(PCWSTR pszVhdPath)
{
	BOOL bRet = FALSE;
	DWORD ret;
	DETACH_VIRTUAL_DISK_FLAG Flags;
	HANDLE hVhd = INVALID_HANDLE_VALUE;
	OPEN_VIRTUAL_DISK_PARAMETERS oparams;
	VIRTUAL_STORAGE_TYPE            vst =
	{
		VIRTUAL_STORAGE_TYPE_DEVICE_VHD,
		VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT
	};

	wprintf(L"OpenAndDetachVHD %s\n", pszVhdPath);

	oparams.Version = OPEN_VIRTUAL_DISK_VERSION_1;
	oparams.Version1.RWDepth = OPEN_VIRTUAL_DISK_RW_DEPTH_DEFAULT;

	ret = OpenVirtualDisk(&vst, pszVhdPath,
		VIRTUAL_DISK_ACCESS_DETACH,
		OPEN_VIRTUAL_DISK_FLAG_NONE, NULL /*&oparams*/, &hVhd);

	if (ERROR_SUCCESS == ret)
	{
		printf("success opening vdisk...\n");
		Flags = DETACH_VIRTUAL_DISK_FLAG_NONE;
		ret = DetachVirtualDisk(hVhd, Flags, 0);
		if (ERROR_SUCCESS == ret)
		{
			printf("success detaching vdisk...\n");
		}
		else
		{
			printf("failed to detach vdisk... %d\n", ret);
			PrintErrorMessage(GetLastError());
			bRet = FALSE;
		}
	}
	else
	{
		printf("failed to open vdisk...err %d\n", ret);
		PrintErrorMessage(GetLastError());
		bRet = FALSE;
	}

	if (INVALID_HANDLE_VALUE != hVhd)
	{
		CloseHandle(hVhd);
	}

	return bRet;
}

BOOL OpenAndCompactVHD(PCWSTR pszVhdPath)
{
	BOOL bRet = FALSE;
	DWORD ret;
	HANDLE hVhd = INVALID_HANDLE_VALUE;
	OPEN_VIRTUAL_DISK_PARAMETERS oparams;
	COMPACT_VIRTUAL_DISK_PARAMETERS parameters;
	COMPACT_VIRTUAL_DISK_FLAG flags = COMPACT_VIRTUAL_DISK_FLAG_NONE;

	VIRTUAL_STORAGE_TYPE            vst =
	{
		VIRTUAL_STORAGE_TYPE_DEVICE_VHD,
		VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT
	};

	wprintf(L"OpenAndCompactVHD %s\n", pszVhdPath);

	oparams.Version = OPEN_VIRTUAL_DISK_VERSION_1;
	oparams.Version1.RWDepth = OPEN_VIRTUAL_DISK_RW_DEPTH_DEFAULT;

	ret = OpenVirtualDisk(&vst, pszVhdPath,
		VIRTUAL_DISK_ACCESS_METAOPS,
		OPEN_VIRTUAL_DISK_FLAG_NONE, &oparams, &hVhd);

	if (ERROR_SUCCESS == ret)
	{
		printf("success opening vdisk...\n");

		parameters.Version = COMPACT_VIRTUAL_DISK_VERSION_1;
		parameters.Version1.Reserved = 0;

		ret = CompactVirtualDisk(hVhd,
			COMPACT_VIRTUAL_DISK_FLAG_NONE,
			&parameters,
			0);

		if (ERROR_SUCCESS == ret)
		{
			printf("success expanding vdisk...\n");
		}
		else
		{
			printf("failed to expand vdisk... %d\n", ret);
			PrintErrorMessage(GetLastError());
			bRet = FALSE;
		}
	}
	else
	{
		printf("failed to open vdisk...err %d\n", ret);
		PrintErrorMessage(GetLastError());
		bRet = FALSE;
	}

	if (INVALID_HANDLE_VALUE != hVhd)
	{
		CloseHandle(hVhd);
	}

	return bRet;
}

BOOL OpenAndExpandVHD(PCWSTR pszVhdPath, ULONG newSizeMB)
{
	// Expanding a virtual disk requires that the virtual disk be detached during 
	// the operation.
	BOOL bRet = FALSE;
	DWORD ret;
	HANDLE hVhd = INVALID_HANDLE_VALUE;
	EXPAND_VIRTUAL_DISK_PARAMETERS xparams;
	VIRTUAL_STORAGE_TYPE            vst =
	{
		VIRTUAL_STORAGE_TYPE_DEVICE_VHD,
		VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT
	};

	wprintf(L"OpenAndExpandVHD %s, new size (MB) %d\n", pszVhdPath, newSizeMB);

	ret = OpenVirtualDisk(&vst, pszVhdPath,
		VIRTUAL_DISK_ACCESS_METAOPS,
		OPEN_VIRTUAL_DISK_FLAG_NONE, NULL, &hVhd);

	if (ERROR_SUCCESS == ret)
	{
		printf("success opening vdisk...\n");
		xparams.Version = EXPAND_VIRTUAL_DISK_VERSION_1;
		xparams.Version1.NewSize = newSizeMB * 1024 * 1024;

		ret = ExpandVirtualDisk(hVhd, EXPAND_VIRTUAL_DISK_FLAG_NONE, &xparams, 0);

		if (ERROR_SUCCESS == ret)
		{
			printf("success expanding vdisk...\n");
		}
		else
		{
			printf("failed to expand vdisk... %d\n", ret);
			PrintErrorMessage(GetLastError());
			bRet = FALSE;
		}
	}
	else
	{
		printf("failed to open vdisk...err %d\n", ret);
		PrintErrorMessage(GetLastError());
		bRet = FALSE;
	}

	if (INVALID_HANDLE_VALUE != hVhd)
	{
		CloseHandle(hVhd);
	}

	return bRet;
}

BOOL OpenAndMergeVHD(PCWSTR pszVhdPath)
{
	BOOL bRet = FALSE;
	DWORD ret;
	HANDLE hVhd;
	MERGE_VIRTUAL_DISK_PARAMETERS mparms;
	OPEN_VIRTUAL_DISK_PARAMETERS oparms;
	MERGE_VIRTUAL_DISK_FLAG flags = MERGE_VIRTUAL_DISK_FLAG_NONE;
	VIRTUAL_STORAGE_TYPE            vst =
	{
		VIRTUAL_STORAGE_TYPE_DEVICE_VHD,
		VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT
	};

	wprintf(L"OpenAndMergeVHD %s\n", pszVhdPath);

	oparms.Version = OPEN_VIRTUAL_DISK_VERSION_1;
	oparms.Version1.RWDepth = 2;

	ret = OpenVirtualDisk(&vst, pszVhdPath,
		VIRTUAL_DISK_ACCESS_METAOPS | VIRTUAL_DISK_ACCESS_GET_INFO,
		OPEN_VIRTUAL_DISK_FLAG_NONE, &oparms, &hVhd);

	if (ERROR_SUCCESS == ret)
	{
		printf("success opening vdisk...\n");

		mparms.Version = MERGE_VIRTUAL_DISK_VERSION_1;
		mparms.Version1.MergeDepth = oparms.Version1.RWDepth - 1; //MERGE_VIRTUAL_DISK_DEFAULT_MERGE_DEPTH;

		ret = MergeVirtualDisk(hVhd, flags, &mparms, NULL);

		if (ERROR_SUCCESS == ret)
		{
			printf("success merging vdisk...\n");
			bRet = TRUE;
		}
		else
		{
			printf("failed to expand vdisk... %d\n", ret);
			PrintErrorMessage(GetLastError());
			bRet = FALSE;
		}
	}
	else
	{
		printf("failed to open vdisk...err %d\n", ret);
		PrintErrorMessage(GetLastError());
		bRet = FALSE;
	}

	if (INVALID_HANDLE_VALUE != hVhd)
	{
		CloseHandle(hVhd);
	}

	return bRet;
}

int OpenAndSetVHDInfo(PCWSTR pszVhdPath, PCWSTR pszGuid, GUID *Guid)
{
	BOOL bRet = FALSE;
	DWORD ret;
	HANDLE hVhd;
	ULONG InfoSize;
	SET_VIRTUAL_DISK_INFO SetInfo;
	VIRTUAL_STORAGE_TYPE            vst =
	{
		VIRTUAL_STORAGE_TYPE_DEVICE_VHD,
		VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT
	};

	wprintf(L"OpenAndSetVHDInfo %s GUID %s\n", pszVhdPath, pszGuid);

	ret = OpenVirtualDisk(&vst, pszVhdPath,
		VIRTUAL_DISK_ACCESS_ALL, OPEN_VIRTUAL_DISK_FLAG_NONE,
		NULL, &hVhd);

	if (ERROR_SUCCESS == ret)
	{
		printf("success opening vdisk...\n");

		SetInfo.Version = SET_VIRTUAL_DISK_INFO_IDENTIFIER;
		InfoSize = sizeof(SetInfo);
		SetInfo.UniqueIdentifier = zGuid; //*Guid;

		ret = SetVirtualDiskInformation(hVhd, &SetInfo);
		if (ret == ERROR_SUCCESS)
		{
			printf("success setting vhd info\n");
		}
		else
		{
			printf("failed to set vhd info %d\n", ret);
			PrintErrorMessage(GetLastError());
		}
	}
	else
	{
		printf("failed to open vdisk...err %d\n", ret);
		PrintErrorMessage(GetLastError());
	}

	if (INVALID_HANDLE_VALUE != hVhd)
	{
		CloseHandle(hVhd);
	}

	return bRet;
}

BOOL OpenAndGetPhysVHD(PCWSTR pszVhdPath, PWSTR pszPhysicalDiskPath)
{
	BOOL bRet = FALSE;
	HANDLE hVhd = INVALID_HANDLE_VALUE;
	DWORD ret;
	OPEN_VIRTUAL_DISK_PARAMETERS oparams;
	ATTACH_VIRTUAL_DISK_PARAMETERS iparams;
	VIRTUAL_STORAGE_TYPE            vst =
	{
		VIRTUAL_STORAGE_TYPE_DEVICE_VHD,
		VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT
	};

	wprintf(L"OpenAndGetPhysVHD %s\n", pszVhdPath);

	oparams.Version = OPEN_VIRTUAL_DISK_VERSION_1;
	oparams.Version1.RWDepth = OPEN_VIRTUAL_DISK_RW_DEPTH_DEFAULT;

	iparams.Version = ATTACH_VIRTUAL_DISK_VERSION_1;

	ret = OpenVirtualDisk(&vst, pszVhdPath,
		VIRTUAL_DISK_ACCESS_ATTACH_RW | VIRTUAL_DISK_ACCESS_GET_INFO | VIRTUAL_DISK_ACCESS_DETACH,
		OPEN_VIRTUAL_DISK_FLAG_NONE, &oparams, &hVhd);

	if (ERROR_SUCCESS == ret)
	{
		ULONG sizePhysicalDisk;
		printf("success opening vdisk...\n");
		memset(pszPhysicalDiskPath, 0, sizeof(wchar_t) * PHYS_PATH_LEN);
		sizePhysicalDisk = (PHYS_PATH_LEN * sizeof(wchar_t)) * 256;
		ret = GetVirtualDiskPhysicalPath(hVhd, &sizePhysicalDisk, pszPhysicalDiskPath);
		if (ERROR_SUCCESS == ret)
		{
			wprintf(L"success getting physical path %s vhdname\n", pszPhysicalDiskPath);
			bRet = TRUE;
		}
		else
		{
			printf("failed to get vhd physical info %d\n", ret);
			PrintErrorMessage(GetLastError());
		}
	}
	else
	{
		printf("failed to open vdisk...err 0x%x\n", ret);
		PrintErrorMessage(GetLastError());
	}

	if (INVALID_HANDLE_VALUE != hVhd)
	{
		CloseHandle(hVhd);
	}

	return bRet;
}

//nie dziala xD
void SetVolumeLetter() 
{
	WCHAR VolumeName[MAX_PATH] = L"";
	
	DWORD bytesReturned;
	VOLUME_DISK_EXTENTS diskExtents;
	HANDLE hFVol = FindFirstVolume(VolumeName, sizeof(VolumeName));
	bool hadTrailingBackslash = false;

	std::wstring wstrVolumeName = std::wstring(VolumeName);

	do {
		// I had a problem where CreateFile complained about the trailing \ and
		// SetVolumeMountPoint desperately wanted the backslash there. I ended up 
		// doing this to get it working but I'm not a fan and I'd greatly 
		// appreciate it if someone has any further info on this matter
		int backslashPos = wstrVolumeName.length() - 1;
		if (hadTrailingBackslash = VolumeName[backslashPos] == '\\') {
			VolumeName[backslashPos] = 0;
		}

		HANDLE hVol = CreateFile(VolumeName, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
		if (hVol == INVALID_HANDLE_VALUE) {
			return;
		}

		DeviceIoControl(hVol, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, NULL,
			0, &diskExtents, sizeof(diskExtents), &bytesReturned, NULL);

		// If the volume were to span across multiple physical disks, you'd find 
		// more than one Extents here but we don't have to worry about that with VHD
		// Note that 'driveNumber' would be the integer you extracted out of 
		// 'physicalDrive' in the previous snippet
		if (diskExtents.Extents[0].DiskNumber == CountPhysicalDisks()) {
			if (hadTrailingBackslash) {
				VolumeName[backslashPos] = '\\';
			}

			// Found volume that's on the VHD, let's mount it with a letter of our choosing.
			// Warning: SetVolumeMountPoint requires elevation
			SetVolumeMountPoint(L"H:\\", VolumeName);
		}
	} while (FindNextVolume(hFVol, VolumeName, sizeof(VolumeName)));
	FindVolumeClose(hFVol);
}