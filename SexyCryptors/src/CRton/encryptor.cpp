#include "common.h"

namespace CRton
{
// Rton Encryptor 

uint8_t* Encryptor::encryptData(uint8_t* input, size_t len, size_t& outLen)
{
        size_t keyLen = 0;
		const uint8_t* key = getKeyHash(&keyLen);
		
		size_t ivLen = 0;
		const uint8_t* iv = initVector(key, keyLen, &ivLen);
		
		size_t paddedLen = Input::getPaddedLen(len, BLOCK_SIZE);
		Crypto::checkBlock(&input, &len, paddedLen);
		
		auto result = RijndaelEncryptor::encryptData(input, len, CIPHER_MODE, BLOCK_SIZE, 
		                                             key, keyLen, iv, ivLen, outLen);
		
		free(key);
		
		return result;
}

    void Encryptor::encryptStream(ReadOnlyStream& input, WriteOnlyStream& output)
	{
		outStream.WriteUInt16(HEADER);
		
	  size_t outLen = 0;
	  auto encryptedData = encryptData(input.Data(), input.Size(), &outLen);
	  
	  outStream.WriteBytes(encryptedData, outLen);
	  free(encryptedData);
	}

}

}

extern "C"
{

void CRton_EncryptFile(char* input, char* output)
{
auto inStream = fileMgr::openRead(input);
auto outStream = fileMgr::openWrite(output);

CRtonEncryptor::encryptStream(*inStream, *outStream);
}					  

}