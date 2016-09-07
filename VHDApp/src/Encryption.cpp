#include <iostream>
#include <string>
#include <memory.h>
#include <windows.h>
#include <wincrypt.h>
#include <mbstring.h>

#include "Encryption.h"

using namespace std;

/*
Acquire key container handle
*/
HCRYPTPROV GetCryptContainer(LPCWSTR keyContainer)
{
	HCRYPTPROV hCryptProv	= NULL;

	if(CryptAcquireContext(
		  &hCryptProv, 
		  keyContainer, 
		  NULL, 
		  PROV_RSA_FULL, 
		  0))
	{
	   printf("A cryptographic provider has been acquired.\n");
	}
	else
	{
		if (GetLastError() == NTE_BAD_KEYSET)
		{
		   if(CryptAcquireContext(
			  &hCryptProv, 
			  keyContainer, 
			  NULL, 
			  PROV_RSA_FULL, 
			  CRYPT_NEWKEYSET)) 
			{
			  printf("A new key container has been created.\n");
			}
			else
			{
			  printf("Could not create a new key container.\n");
			}
		}
		else
		{
			printf("A cryptographic service handle could not be acquired.\n");
		}
	}

	return hCryptProv;
}

HCRYPTHASH CreateHashObj(HCRYPTPROV hCryptProv, const char * szPassword)
{
	HCRYPTHASH hHash = NULL;

	if(CryptCreateHash(
       hCryptProv, 
       CALG_MD5, 
       0, 
       0, 
       &hHash))
    {
        printf("A hash object has been created. \n");

		//  hash the password
		if(CryptHashData(
			hHash, 
			(BYTE *)szPassword, 
			strlen(szPassword), 
			0))
		 {
			printf("The password has been added to the hash. \n");

		 }
		 else
		 {
			 // reset hash object to NULL
			CryptDestroyHash(hHash);
			hHash = NULL;		
		 }
    }
    else
    { 
         printf("Failed to create hash object.\n");
    }

	return hHash;
}

// Derive a session key from the hash object. 
HCRYPTKEY GetDeriveKey(HCRYPTPROV hCryptProv, HCRYPTHASH hHash)
{
	HCRYPTKEY hKey = NULL;

	if(CryptDeriveKey(
		   hCryptProv, 
		   ENCRYPT_ALGORITHM, 
		   hHash, 
		   KEYLENGTH, 
		   &hKey))
	 {
	   printf("An encryption key is derived from the password hash. \n"); 
	 }
	 else
	 {
	   //MyHandleError("Error during CryptDeriveKey!\n"); 
	 }
	return hKey;
}

string Encrypt(const char* password, const char* toEncrypt)
{
	HCRYPTPROV hCryptProv = NULL;				// handle for a cryptographic 
												// provider context.
	HCRYPTHASH hHash = NULL;				// handle for hash object

	LPCWSTR keyContainer = L"MyKeyContainer";	// name of the key container
											// to be used.
	HCRYPTKEY hKey;								// handle for session key
	PBYTE pbBuffer;								// hold the data being encrypt 
												// or decrypt
	DWORD dwCount;								// data length

												// make sure that we have enough argument to do the demo.
												// otherwise exit the program
												/*if(argc < 2)
												{
												printf("Usage: CryptoConsole.exe [data]");
												exit(1);
												}*/

												// get CSP
	hCryptProv = GetCryptContainer(keyContainer);

	if (!hCryptProv)
		exit(1); // exit if failed to get CSP 

				 // get the hash object
	hHash = CreateHashObj(hCryptProv, password);

	if (!hHash)
		exit(1); // exit if failed to get hash object

				 // get the session key
	hKey = GetDeriveKey(hCryptProv, hHash);

	if (!hKey)
		exit(1); // exit if failed to get session key

				 // determine input data length
	size_t len = strlen(toEncrypt);

	// Determine the block size. If a block cipher is used, 
	// it must have room for an extra block. 
	DWORD dwBufferLen = len + ENCRYPT_BLOCK_SIZE - (len % ENCRYPT_BLOCK_SIZE);

	// Allocate memory
	if (pbBuffer = (BYTE *)malloc(dwBufferLen))
	{
		printf("Memory has been allocated for the buffer. \n");
	}
	else
	{
		printf("Failed to allocate memory for the buffer. \n");
		exit(1);
	}

	// copy the data value to buffer
	memcpy(pbBuffer, toEncrypt, dwBufferLen);
	printf("++++++++++++++++++++++++++++++++\n");
	printf("Before encryption: %s\n", toEncrypt);
	dwCount = strlen(toEncrypt);

	string encryptedText;

	// encrypt data
	if (!CryptEncrypt(
		hKey,
		0,
		TRUE,
		0,
		pbBuffer,
		&dwCount,
		dwBufferLen))
	{
		printf("Error during CryptEncrypt. \n");
	}
	
	else
	{
		printf("After encryption: %s\n", pbBuffer);

		unsigned char* pDecrypt = pbBuffer;
		DWORD delen = _mbslen(pbBuffer);

		encryptedText = string(reinterpret_cast<char*>(pDecrypt));
	}

	// Remove all references
	if (pbBuffer)
		free(pbBuffer);
	if (hKey)
		CryptDestroyKey(hKey);
	if (hHash)
		CryptDestroyHash(hHash);
	if (hCryptProv)
		CryptReleaseContext(hCryptProv, 0);

	return encryptedText;
}