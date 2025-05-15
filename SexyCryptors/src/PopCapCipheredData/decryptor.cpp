#include "common.h"

namespace Cdat
{
// Cdat Decryptor 

    void Decryptor::decryptStream(ReadOnlyStream& input, WriteOnlyStream& output, ProgressCallback progress)
	{
		bool validHeader = input.CompareStr(HEADER, progress);

        if(!validHeader)
		return;
	
	  bool validSize = input.CheckSize64(input.Size(), progress);
	  
	  if(!validSize)
	  return;
	  
	  if(input.Size() >= BYTES_TO_CIPHER + strlen(HEADER) + sizeof(uint64_t) )
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

void Cdat_DecryptFile(char* input, char* output, ProgressCallback progress)
{
auto inStream = fileMgr::openRead(input);
auto outStream = fileMgr::openWrite(output);

CRtonDecryptor::decryptStream(*inStream, *outStream, progress);
}					  

}