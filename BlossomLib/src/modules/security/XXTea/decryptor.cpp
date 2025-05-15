#include "common.h"

namespace XXTea
{
// XXTea Decryptor

uint8_t* Decryptor::decryptData(uint8_t* input, size_t len, uint8_t* key, size_t keyLen, size_t& outLen, ProgressCallback progress)
{
    initParams(&input, &len, &key, &keyLen);

    uint32_t* v = toUInt32(input, len, false);
    int n = v[0] - 1;

    uint32_t* k = toUInt32(key, keyLen, false);

    int totalRounds = Q(n);
    uint32_t z, y = v[0], sum = (uint32_t)(totalRounds * DELTA), e;
    int p;

    for (int i = 0; i < totalRounds; ++i)
    {
        e = E(sum);

        for (p = n; p > 0; p--)
        {
            z = v[p - 1];
            y = v[p] -= MX(sum, y, z, p, e, k);
        }

        z = v[n];
        y = v[0] -= MX(sum, y, z, p, e, k);

        sum -= DELTA;

        if (progress)
            progress(i+1, totalRounds, "ACTION_DECRYPT_DATA", false);
        
    }

    uint8_t* result = toBytes(v, n + 1, true);
    outLen = n << 2;

    free(v);
    free(k);

    return result;
}

	
uint8_t* Decryptor::decryptStream(ReadOnlyStream& input, uint8_t* key, size_t keyLen, size_t& outLen, ProgressCallback progress)
	{
	  return decryptData(input.Data(), input.Size(), key, keyLen, outLen, progress);
	}

}

extern "C"
{

uint8_t* XXTea_DecryptData(uint8_t* input, int64_t len, uint8_t* key, int64_t keyLen, int& outLen)
{
return XXTeaDecryptor::decryptData(input, len, key, keyLen, outLen, nullptr);
}

void XXTea_DecryptFile(char* input, char* output, uint8_t* key, int64_t keyLen, ProgressCallback progress)
{
auto inStream = fileMgr::openRead(input);

size_t outLen = 0;
uint8_t* decryptedData = XXTeaDecryptor::decryptStream(*inStream, key, keyLen, &outLen, progress);

auto outStream = fileMgr::openWrite(output);
outStream->WriteBytes(decryptedData, outLen);

free(decryptedData);
}

}