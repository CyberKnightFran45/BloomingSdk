#include "common.h"

namespace base64
{
// Base64 Decoder

void Decoder::initReverseTable(const char* alphabet) 
{
	
    if (reverse_initialized) 
		return;

    for (int i = 0; i < 256; i++) 
		reverse_table[i] = 0xFF;

    for (int i = 0; i < 64; i++) {
        reverse_table[static_cast<uint8_t>(alphabet[i])] = i;
    }

    reverse_initialized = true;
}

uint8_t* Decoder::decodeData(const char* input, size_t len, bool webSafe, size_t* outLen, ProgressCallback progress) 
{
    const char* alphabet = webSafe ? ALPHABET_WEB : ALPHABET;
    initReverseTable(alphabet);

    if (len % 4 != 0) 
		return nullptr;

    size_t padding = 0;
    if (len >= 1 && input[len - 1] == '=') padding++;
    if (len >= 2 && input[len - 2] == '=') padding++;

    size_t decoded_len = (len / 4) * 3 - padding;
    auto output = (uint8_t*)malloc(decoded_len);
	
    if (!output) 
	return nullptr;

    size_t j = 0;
	
    for (size_t i = 0; i < len;)
		{
			
			if(progress)
	progress(i+1, len, "ACTION_DECODE_DATA", false);	
			
        uint32_t sextet_a = reverse_table[static_cast<uint8_t>(input[i++])];
        uint32_t sextet_b = reverse_table[static_cast<uint8_t>(input[i++])];
        uint32_t sextet_c = reverse_table[static_cast<uint8_t>(input[i++])];
        uint32_t sextet_d = reverse_table[static_cast<uint8_t>(input[i++])];

        if (sextet_a > 63 || sextet_b > 63 || (sextet_c > 63 && input[i - 2] != '=') || (sextet_d > 63 && input[i - 1] != '=') )
		{
			progress(0, -1, "ERROR_INVALID_BASE64_CHARS", true);
            return nullptr;
		}

        uint32_t triple = (sextet_a << 18) | (sextet_b << 12) | ((sextet_c & 0x3F) << 6) | (sextet_d & 0x3F);

        if (j < decoded_len) output[j++] = (triple >> 16) & 0xFF;
        if (j < decoded_len) output[j++] = (triple >> 8) & 0xFF;
        if (j < decoded_len) output[j++] = triple & 0xFF;
    }

    if (outLen) *outLen = decoded_len;
    return output;
}

void Decoder::decodeStream(ReadOnlyStream& input, WriteOnlyStream& output, bool webSafe, int64_t maxBytes, ProgressCallback progress)
{

auto transform = [webSafe](const uint8_t* inData, size_t inSize, void*& outData) -> size_t
 {
        size_t decodedSize = 0;
        outData = decodeData(static_cast<char*>(inData), inSize, webSafe, &decodedSize, progress);
		
        return decodedSize;
    };

fileMgr::process(input, output, maxBytes, transform);
}

}

extern "C"
{

char* Base64_DecodeData(const uint8_t* input, int64_t len, bool webSafe, int64_t* outLen, ProgressCallback progress)
{
return Base64Decoder::decodeData(input, static_cast<size_t>(len), webSafe, static_cast<size_t>(outLen), progress);
}

void Base64_DecodeFile(const char* input, const char* output, bool webSafe, int64_t maxBytes, ProgressCallback progress)
{
    auto inStream = fileMgr::openRead(input);
    auto outStream = fileMgr::openWrite(output);

    Base64Decoder::decodeStream(*inStream, *outStream, webSafe, maxBytes, progress);
}


}