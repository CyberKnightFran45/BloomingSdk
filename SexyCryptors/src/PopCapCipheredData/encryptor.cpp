#include "common.h"

namespace Cdat
{
// Cdat Encryptor 

    void Encryptor::encryptStream(ReadOnlyStream& input, WriteOnlyStream& output, ProgressCallback progress)
	{
		outStream.WriteString(HEADER);
		outStream.WriteUInt64(static_cast<uint64_t>(input.Size() ) );
		
		if(input.Size() >= BYTES_TO_CIPHER)
		{
		   size_t keyLen = 0;
	       const uint8_t* key = getRawKey(&keyLen);
		  
           XorCryptor::cipherStream(input, output, key, keyLen, BYTES_TO_CIPHER, progress);	
		   free(key);
		}
		
       fileMgr.process(input, output, -1, progress);
	}

}

extern "C"
{

void Cdat_EncryptFile(char* input, char* output, ProgressCallback progress)
{
auto inStream = fileMgr::openRead(input);
auto outStream = fileMgr::openWrite(output);

CdatEncryptor::encryptStream(*inStream, *outStream, progress);
}					  

}