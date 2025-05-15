#include "common.h"

namespace rijndael
{
// Rijndael Encryptor 

uint8_t* Encryptor::encryptData(uint8_t* input, size_t len, int mode, int blockSize, uint8_t* key,
                                       size_t keyLen, uint8_t* iv, size_t ivLen, size_t& outLen)
{
initParams(&input, &len, &blockSize, &key, &keyLen, &iv, &ivLen);
outLen = len;

auto result = static_cast<char*>(std::malloc(outLen) );
rCryptor.Encrypt(static_cast<char*>(input), result, len, mode);

return static_cast<uint8_t*>(result);
}

uint8_t* Encryptor::encryptStream(ReadOnlyStream& input, int mode, int blockSize, uint8_t* key, 
                                  size_t keyLen, uint8_t* iv, size_t ivLen, size_t& outLen)
{
return encryptData(input.Data(), input.Size(), mode, blockSize, key, keyLen, iv, ivLen, outLen);
}

}

extern "C" 
{
	
uint8_t* Rijndael_EncryptData(uint8_t* input, uint64_t len, int mode, int blockSize, uint8_t* key, 
                              uint64_t keyLen, uint8_t* iv, uint64_t ivLen, size_t& outLen)
{
return RijndaelEncryptor::encryptData(input, len, mode, blockSize, key, keyLen, iv, ivLen, outLen);
}											 

void Rijndael_EncryptFile(char* input, char* output, int mode, int blockSize, 
                          uint8_t* key, int64_t keyLen, uint8_t* iv, int64_t ivLen)
{
	auto inStream = fileMgr::openRead(input);
    auto outStream = fileMgr::openWrite(output);
   
   size_t outLen = 0;
   uint8_t* encryptedData =  RijndaelEncryptor::encryptStream(*inStream, mode, blockSize, key, keyLen, iv, ivLen, &outLen);
   
   outStream->WriteBytes(encryptedData, outLen);
   free(encryptedData);
}

}