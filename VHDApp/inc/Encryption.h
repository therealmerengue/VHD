#pragma once
#include <iostream>
#include <string>
#include <memory.h>
#include <windows.h>
#include <wincrypt.h>
#include <mbstring.h>

#define ENCRYPT_ALGORITHM CALG_RC4 
#define KEYLENGTH  0x00800000
#define ENCRYPT_BLOCK_SIZE 8 

using namespace std;

namespace Encryption
{
	HCRYPTPROV GetCryptContainer(LPCWSTR keyContainer); //Acquire key container handle

	HCRYPTHASH CreateHashObj(HCRYPTPROV hCryptProv, const char* szPassword);

	HCRYPTKEY GetDeriveKey(HCRYPTPROV hCryptProv, HCRYPTHASH hHash); // Derive a session key from the hash object. 

	string Encrypt(const char* password, const char* toEncrypt);
}