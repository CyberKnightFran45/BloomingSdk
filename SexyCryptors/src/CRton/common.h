#include "BlosomLib/src/global/lib_export.h"
#include "BlosomLib/src/global/utils/crypto/params.h"
#include "BlosomLib/src/global/utils/input/helper.h"
#include "BlosomLib/src/modules/file_manager.hpp"
#include "BlosomLib/src/modules/security/digest/generic.h"
#include <memory>
#include <stdexcept>
#include <cstring>
#include <iostream>

namespace SexyCryptors::RtonCryptor
{
// Internal defs

struct Common 
{
private:
static const char* CIPHER_KEY = "com_popcap_pvz2_magento_product_2013_05_05";

protected:
static constexpr uint16_t HEADER = 16;

static constexpr int CIPHER_MODE = 1; // CBC
static constexpr size_t BLOCK_SIZE = 24;

static const uint8_t* getKeyHash(size_t& keyLen)
{
char* hashStr = digest::getHash(key, keyLen, "MD5");
uint8_t* hashBytes = Input::getUtf8Bytes(hashStr, keyLen);

free(hashStr);

return hashBytes;
}

static const uint8_t* initVector(uint8_t* key, size_t keyLen, size_t& ivLen)
{
return Crypto::initVector(key, keyLen, BLOCK_SIZE, 4, ivLen);
}

};	

struct Encryptor : protected Common
{
static void encryptStream(ReadOnlyStream& input, WriteOnlyStream& output);

private:
static uint8_t* encryptData(uint8_t* input, size_t len, size_t& outLen);
};

struct Decryptor : protected Common
{
static void decryptStream(ReadOnlyStream& input, WriteOnlyStream& output);

private:
static uint8_t* decryptData(uint8_t* input, size_t len, size_t& outLen);
};

}

namespace CRton = SexyCryptors::RtonCryptor;

using CRtonEncryptor = CRton::Encryptor;
using CRtonDecryptor = CRton::Decryptor;
	
extern "C"
{			  
DLL_EXPORT void CRton_EncryptFile(char* input, char* output);
DLL_EXPORT void CRton_DecryptFile(char* input, char* output);
}