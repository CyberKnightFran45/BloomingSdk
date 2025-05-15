#include "../../../global/lib_export.h"
#include "../../../global/callback.h"
#include "../../../global/utils/crypto/params.h"
#include "../../../global/utils/input/helper.h"
#include "../../file_manager.hpp"
#include <memory>
#include <stdexcept>
#include <cstring>
#include <iostream>

namespace BlossomLib::Modules::Security::XXTea
{
// Internal defs

struct Common 
{
protected:

static constexpr uint32_t DELTA = 0x9E3779B9;

 static  uint32_t MX(uint32_t sum, uint32_t y, uint32_t z, int p, uint32_t e, const uint32_t* k)
    {
        int pe = (p & 3 ^ e);
        return (z >> 5 ^ y << 2) + (y >> 3 ^ z << 4) ^ (sum ^ y) + (k[pe] ^ z);
    }

 static int Q(size_t n)
    {
        return 6 + 52 / (n + 1);
    }

  static  uint32_t E(uint32_t sum)
    {
        return sum >> 2 & 3;
    }

  static uint32_t* toUInt32(const uint8_t* data, size_t length, bool includeLen)
    {
        int n = (length & 3) == 0 ? length >> 2 : (length >> 2) + 1;

        size_t cipheredLen = includeLen ? n + 1 : n;
        auto result = static_cast<uint32_t*>(std::malloc(cipheredLen * sizeof(uint32_t) ) );

        if (includeLen)
            result[n] = (uint32_t)length;
        
        for (size_t i = 0; i < length; ++i)
            result[i >> 2] |= (uint32_t)data[i] << ((i & 3) << 3);

        return result;
    }

   static uint8_t* toBytes(const uint32_t* data, size_t length, bool includeLen)
    {
        size_t n = length << 2;
		
        if (includeLen)
        {
            int m = (int)data[length - 1];
            n -= 4;
			
            if (m < n - 3 || m > n)
             return nullptr;
			 
            n = m;
        }

        auto result = static_cast<uint8_t*>(std::malloc(n) );

        for (size_t i = 0; i < n; ++i)
            result[i] = (uint8_t)(data[i >> 2] >> ((i & 3) << 3) );
        
        return result;
    }
	
	static void initParams(uint8_t* &data, size_t &dataLen, uint8_t* &key, size_t &keyLen)
	{
	    size_t paddedLen = Input::getPaddedLen(dataLen);
	 
        Crypto::checkBlock(data, dataLen, paddedLen);
        Crypto::checkBlock(key, keyLen, 16);
	}

};

struct Encryptor : protected Common
{
static  uint8_t* encryptData(uint8_t* input, size_t len, uint8_t* key, size_t keyLen, size_t& outLen, ProgressCallback progress);
static uint8_t* encryptStream(ReadOnlyStream& input, uint8_t* key, size_t keyLen, size_t& outLen, ProgressCallback progress);
};	

struct Decryptor : protected Common
{
static  uint8_t* decryptData(uint8_t* input, size_t len, uint8_t* key, size_t keyLen, size_t& outLen, ProgressCallback progress);
static uint8_t* decryptStream(ReadOnlyStream& input, uint8_t* key, size_t keyLen, size_t& outLen, ProgressCallback progress);
};	

}

namespace XXTea = BlossomLib::Modules::Security::XXTea;

using XXTeaEncryptor = XXTea::Encryptor;
using XXTeaDecryptor = XXTea::Decryptor;
	
extern "C"
{
DLL_EXPORT uint8_t* XXTea_EncryptData(uint8_t* input, int64_t len, uint8_t* key, int64_t keyLen, int& outLen);				  
DLL_EXPORT void XXTea_EncryptFile(char* input, char* output, uint8_t* key, int64_t keyLen, ProgressCallback progress);						  

DLL_EXPORT uint8_t* XXTea_DecryptData(uint8_t* input, int64_t len, uint8_t* key, int64_t keyLen, int& outLen);
DLL_EXPORT void XXTea_DecryptFile(char* input, char* output, uint8_t* key, int64_t keyLen, ProgressCallback progress);	
}