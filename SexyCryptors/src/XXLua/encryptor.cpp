#include "common.h"

namespace XXLua
{
// XXLua Encryptor 

uint8_t* Encryptor::encryptData(uint8_t* input, size_t len, size_t& outLen, ProgressCallback progress)
{
        size_t keyLen = 0;
		const uint8_t* key = getRawKey(&keyLen);
		
        auto result = XXTeaEncryptor::encryptData(input, len, key, keyLen, outLen, progress);
		free(key);
		
		return result;
		
    }
	
    void Encryptor::encryptStream(ReadOnlyStream& input, WriteOnlyStream& output, ProgressCallback progress)
	{
		outStream.WriteString(HEADER);
		
	  size_t outLen = 0;
	  auto encryptedData = encryptData(input.Data(), input.Size(), &outLen, progress);
	  
	  outStream.WriteBytes(encryptedData, outLen);
	  free(encryptedData);
	}

}

extern "C"
{

void XXLua_EncryptFile(char* input, char* output, ProgressCallback progress)
{
auto inStream = fileMgr::openRead(input);
auto outStream = fileMgr::openWrite(output);

XXLuaEncryptor::encryptStream(*inStream, *outStream, progress);
}					  

}