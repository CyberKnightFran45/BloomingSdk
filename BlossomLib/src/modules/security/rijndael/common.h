#pragma once

#include "../../../global/lib_export.hpp"
#include "../../../global/utils/crypto/crypto_params.hpp"
#include "../../../plugins/rijndael/Rijndael.h"
#include "../../file_manager.hpp"

namespace BlossomLib::Modules::Security::Rijndael
{
// Internal defs

struct Common 
{
private:

static void checkBlockSize(int& blockSize)
{

if (blockSize == 16 || blockSize == 24 || blockSize == 32) 
    return;

  blockSize = 16;
}
	
protected:
	
static auto rCryptor = CRijndael(); // Plugin instance (Modes: ECB=0, CBC=1, CFB=2)

static void initParams(uint8_t* &data, size_t &dataLen, int &blockSize, uint8_t* &key, 
                       size_t &keyLen, uint8_t* &iv, size_t &ivLen)
{
checkBlockSize(blockSize);

crypto::checkBlock(data, dataLen, blockSize);
crypto::checkBlock(key, keyLen, blockSize);
crypto::checkBlock(iv, ivLen, blockSize);

rCryptor.MakeKey(static_cast<char*>(key), static_cast<char*>(iv), static_cast<int>(keyLen), blockSize);
}

};

struct Encryptor : protected Common
{
static  uint8_t* encryptData(uint8_t* input, size_t len, int mode, int blockSize, uint8_t* key,
                            size_t keyLen, uint8_t* iv, size_t ivLen, size_t& outLen);

static uint8_t* encryptStream(ReadOnlyStream& input, int mode, int blockSize, uint8_t* key, 
                              size_t keyLen, uint8_t* iv, size_t ivLen, size_t& outLen);

};

struct Decryptor : protected Common
{
static  uint8_t* decryptData(uint8_t* input, size_t len, int mode, int blockSize, uint8_t* key,
                             size_t keyLen, uint8_t* iv, size_t ivLen, size_t& outLen);

static uint8_t* decryptStream(ReadOnlyStream& input, int mode, int blockSize, uint8_t* key, 
                              size_t keyLen, uint8_t* iv, size_t ivLen, size_t& outLen);

};

}

namespace rijndael = BlossomLib::Modules::Security::Rijndael;

using RijndaelEncryptor = rijndael::Encryptor;
using RijndaelDecryptor = rijndael::Decryptor;

extern "C" 
{
	
DLL_EXPORT uint8_t* Rijndael_EncryptData(uint8_t* input, int64_t len, int mode, int blockSize, 
                                         uint8_t* key, int64_t keyLen, uint8_t* iv, int ivLen);
	
DLL_EXPORT void Rijndael_EncryptFile(char* input, char* output, int mode, int blockSize,
                                     uint8_t* key, int64_t keyLen, uint8_t* iv, int64_t ivLen);
									 
DLL_EXPORT uint8_t* Rijndael_DecryptData(uint8_t* input, int64_t len, int mode, int blockSize, 
                                         uint8_t* key, int64_t keyLen, uint8_t* iv, int64_t ivLen);									 

DLL_EXPORT void Rijndael_DecryptFile(char* input, char* output, int mode, int blockSize,
                                     uint8_t* key, int64_t keyLen, uint8_t* iv, int64_t ivLen);
}