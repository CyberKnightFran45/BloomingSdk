#include "common.h"

namespace XXLua
{
// XXLua Decryptor

uint8_t* Decryptor::decryptData(uint8_t* input, size_t len, size_t& outLen, ProgressCallback progress)
{
        size_t keyLen = 0;
		const uint8_t* key = getRawKey(&keyLen);
		
        auto result = XXTeaDecryptor::decryptData(input, len, key, keyLen, outLen, progress);
		free(key);
		
		return result;
		
    }
	
void Decryptor::decryptStream(ReadOnlyStream& input, WriteOnlyStream& output, ProgressCallback progress)
{
bool validHeader = input.CompareString(HEADER);

if(!validHeader)
{
	
if(progress)
progress(0, -1, "ERROR_INVALID_XXTEA_HEADER", true);

return;
}

size_t outLen = 0;
auto decryptedData = decryptData(input.Data(), input.Size(), &outLen, progress);

outStream.WriteBytes(decryptedData, outLen);
free(decryptedData);
}

}

extern "C"
{

void XXLua_DecryptFile(char* input, char* output, ProgressCallback progress)
{
auto inStream = fileMgr::openRead(input);
auto outStream = fileMgr::openWrite(output);

XXluaDecryptor::decryptStream(*inStream, *outStream, progress);
}

}