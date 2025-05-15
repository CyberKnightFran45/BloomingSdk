#include "common.h"

namespace base64
{
// Base64 Encoder

char* Encoder::encodeData(const uint8_t* input, size_t len, bool webSafe, size_t* outLen, ProgressCallback progress) 
{
    const char* alphabet = webSafe ? ALPHABET_WEB : ALPHABET;

    size_t enc_len = ( (len + 2) / 3) * 4;
    char* output = (char*)malloc(enc_len + 1);  // +1 for null terminator
	
    if (!output) 
    return nullptr;
	
    size_t j = 0;
	
    for (size_t i = 0; i < len;) 
	{
        uint32_t octet_a = i < len ? input[i++] : 0;
        uint32_t octet_b = i < len ? input[i++] : 0;
        uint32_t octet_c = i < len ? input[i++] : 0;

        uint32_t triple = (octet_a << 16) | (octet_b << 8) | octet_c;

        output[j++] = alphabet[(triple >> 18) & 0x3F];
        output[j++] = alphabet[(triple >> 12) & 0x3F];
        output[j++] = (i > len + 1) ? '=' : alphabet[(triple >> 6) & 0x3F];
        output[j++] = (i > len)     ? '=' : alphabet[triple & 0x3F];
		
				if(progress)
	            progress(i+1, len, "ACTION_ENCODE_DATA", false);	
		
    }

    if (outLen) *outLen = enc_len;
    output[enc_len] = '\0';

    return output;
}

void Encoder::encodeStream(ReadOnlyStream& input, WriteOnlyStream& output, bool webSafe, int64_t maxBytes, ProgressCallback progress)
{

auto transform = [webSafe](uint8_t* data, size_t size, void*& outData) -> size_t
{
            size_t encodedLen = 0;
            outData = encodeData(data, size, webSafe, &encodedLen, progress);
            
            return encodedLen;
        };

fileMgr::process(input, output, maxBytes, transform);
}

}

extern "C"
{

char* Base64_EncodeData(const uint8_t* input, int64_t len, bool webSafe, int64_t* outLen, ProgressCallback progress)
{
return Base64Encoder::encodeData(input, static_cast<size_t>(len), webSafe, static_cast<size_t>(outLen), progress);
}

void Base64_EncodeFile(const char* input, const char* output, bool webSafe, int64_t maxBytes, ProgressCallback progress)
{
    auto inStream = fileMgr::openRead(input);
    auto outStream = fileMgr::openWrite(output);

    Base64Encoder::encodeStream(*inStream, *outStream, webSafe, maxBytes, progress);
}


}