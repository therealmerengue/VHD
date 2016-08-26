#include <windows.h>
#include <wincrypt.h>
#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include "dirent.h"
#include "VHD.h"
#include "Files.h"

void CleanUp(HCRYPTKEY hKey, HCRYPTPROV hProv)
{
	// Destroy session key.
	if (hKey != 0) CryptDestroyKey(hKey);
	// Release provider handle.
	if (hProv != 0) CryptReleaseContext(hProv, 0);
}

int main()
{
	HCRYPTPROV hProv = 0;
	HCRYPTKEY hKey = 0;
	// Get handle to user default provider.
	if (!CryptAcquireContextA(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET)) {
		printf("Error %x during CryptAcquireContext!\n", GetLastError());
		CleanUp(hKey, hProv);
	}
	// Create block cipher session key.
	if (!CryptGenKey(hProv, CALG_RC2, CRYPT_EXPORTABLE, &hKey)) {
		printf("Error %x during CryptGenKey!\n", GetLastError());
		CleanUp(hKey, hProv);
	}

	return 0;
}