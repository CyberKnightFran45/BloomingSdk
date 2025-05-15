#include "common.h"

namespace XXTea
{
// XXTea Encryptor 

uint8_t* Encryptor::encryptData(uint8_t* input, size_t len, uint8_t* key, size_t keyLen, size_t& outLen, ProgressCallback progress)
{
    initParams(&input, &len, &key, &keyLen);

    uint32_t* v = toUInt32(input, len, true);
    int n = v[0] - 1;

    uint32_t* k = toUInt32(key, keyLen, false);

    uint32_t z = v[n], y, sum = 0, e;
    int p, q = Q(n);

    for (int i = 0; i < q; ++i)
    {
        sum += DELTA;
        e = E(sum);

        for (p = 0; p < n; p++)
        {
            y = v[p + 1];
            z = v[p] += MX(sum, y, z, p, e, k);
        }

        y = v[0];
        z = v[n] += MX(sum, y, z, p, e, k);

        if (progress)
            progress(i+1, q, "ACTION_ENCRYPT_DATA", false);
        
		
    }

    uint8_t* result = toBytes(v, n + 1, false);
    outLen = n << 2;

    free(v);
	free(k);
	
    return result;
}

uint8_t* Encryptor::encryptStream(ReadOnlyStream& input, uint8_t* key, size_t keyLen, size_t& outLen, ProgressCallback progress)
	{
	  return encryptData(input.Data(), input.Size(), key, keyLen, outLen, progress);
	}

};

}

extern "C"
{

uint8_t* XXTea_EncryptData(uint8_t* input, int64_t len, uint8_t* key, int64_t keyLen, int& outLen)
{
return XXTeaEncryptor::encryptData(input, len, key, keyLen, outLen, nullptr);
}

void XXTea_EncryptFile(char* input, char* output, uint8_t* key, int64_t keyLen, ProgressCallback progress)
{
auto inStream = fileMgr::openRead(input);

size_t outLen = 0;
uint8_t* encryptedData = XXTeaEncryptor::encryptStream(*inStream, key, keyLen, &outLen, progress);

auto outStream = fileMgr::openWrite(output);
outStream->WriteBytes(encryptedData, outLen);

free(encryptedData);
}					  

}