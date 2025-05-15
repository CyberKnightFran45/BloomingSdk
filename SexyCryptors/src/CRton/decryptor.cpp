#include "common.h"

namespace CRton
{
// Rton Decryptor 

uint8_t* Decryptor::decryptData(uint8_t* input, size_t len, size_t& outLen)
{
	
	if(!Input::checkPadding(len, BLOCK_SIZE) )
		return nullptr;
	
        size_t keyLen = 0;
		const uint8_t* key = getKeyHash(&keyLen);
		
		size_t ivLen = 0;
		const uint8_t* iv = initVector(key, keyLen, &ivLen);
		
		auto result = RijndaelDecryptor::decryptData(input, len, CIPHER_MODE, BLOCK_SIZE, 
		                                             key, keyLen, iv, ivLen, outLen);
		
		free(key);
		
		return result;
}

    void Decryptor::decryptStream(ReadOnlyStream& input, WriteOnlyStream& output)
	{
		bool validHeader = input.CompareUInt16(HEADER);

        if(!validHeader)
        return; // Ignore
		
	  size_t outLen = 0;
	  auto decryptedData = decryptData(input.Data(), input.Size(), &outLen);
	  
	  outStream.WriteBytes(decryptedData, outLen);
	  free(decryptedData);
	}

}

}

extern "C"
{

void CRton_DecryptFile(char* input, char* output)
{
auto inStream = fileMgr::openRead(input);
auto outStream = fileMgr::openWrite(output);

CRtonDecryptor::decryptStream(*inStream, *outStream);
}					  

}