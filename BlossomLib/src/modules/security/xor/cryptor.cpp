#include "cryptor.h"

namespace xOr
{

uint8_t* cipherData(const uint8_t* input, size_t len, const uint8_t* key, size_t keyLen, ProgressCallback progress)
{
    if (!input || !key || len == 0 || keyLen == 0)
        return nullptr;

    auto result = static_cast<uint8_t*>(std::malloc(len));
	
    if (!result) 
	return nullptr;

    for (size_t i = 0; i < len; ++i)
	{
        result[i] = input[i] ^ key[i % keyLen];
		
		if(progress)
		progress(i+1, len, "ACTION_CIPHER_DATA", false);
		
	}
		
    
    return result;
}

inline void cipherStream(ReadOnlyStream& input, WriteOnlyStream& output, const uint8_t* key, 
                         size_t keyLen, int64_t maxBytes, ProgressCallback progress)
{

auto transform = [key, keyLen](const uint8_t* inData, size_t inSize, uint8_t*& outData) -> size_t
{
outData = cipherData(inData, inSize, key, keyLen);

return inSize;
};

fileMgr::process(input, output, maxBytes, transform);
}

}

extern "C" 
{

uint8_t* Xor_CipherData(const uint8_t* input, uint64_t len, const uint8_t* key, uint64_t keyLen)
{
    return xOr::cipherData(input, len, key, keyLen, nullptr);
}

void Xor_CipherFile(char* input, char* output, const uint8_t* key, uint64_t keyLen, int64_t maxBytes, ProgressCallback progress)
{
    auto inStream = fileMgr::openRead(input);
    auto outStream = fileMgr::openWrite(output);

    xOr::cipherStream(*inStream, *outStream, key, keyLen, maxBytes, progress);
}

}
