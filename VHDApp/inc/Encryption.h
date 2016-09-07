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

/*
Acquire key container handle
*/
HCRYPTPROV GetCryptContainer(LPCWSTR keyContainer);

HCRYPTHASH CreateHashObj(HCRYPTPROV hCryptProv, const char * szPassword);

// Derive a session key from the hash object. 
HCRYPTKEY GetDeriveKey(HCRYPTPROV hCryptProv, HCRYPTHASH hHash);

string Encrypt(const char* password, const char* toEncrypt);