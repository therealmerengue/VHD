#include <windows.h>
#include <stdio.h>
#include <winioctl.h>
#include <initguid.h>
#include <virtdisk.h>
#include <commctrl.h>
#include <comdef.h>
#include <set>
#include <string>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <Setupapi.h>
#include <Ntddstor.h>

#include "dirent.h"
#include "StringOperations.h"
#include "VHD.h"

namespace VHD
{
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

			BOOL bResult = FALSE;     // generic results flag   
			HANDLE hDevice;               // handle to the drive to be examined  
			DWORD junk = 0;                   // discard results 

			//Tu jest zmienione:
			string sDiskIndex = std::to_string(diskIndex);
			string deviceName = "\\\\.\\PhysicalDrive" + sDiskIndex;
			std::wstring stemp = String::s2ws(deviceName);
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
}
