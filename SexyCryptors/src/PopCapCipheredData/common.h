#include "BlosomLib/src/global/callback.h"
#include "BlosomLib/src/global/utils/crypto/params.h"
#include "BlosomLib/src/global/utils/input/helper.h"
#include "BlosomLib/src/global/utils/path/helper.h"
#include "BlosomLib/src/modules/security/xor/cryptor.h"
#include <memory>
#include <stdexcept>
#include <cstring>
#include <iostream>

namespace SexyCryptors::PopCapCipheredData
{
// Internal defs

struct Common 
{
private:
static const char* CIPHER_KEY = "AS23DSREPLKL335KO4439032N8345NF";

protected:
static const char* HEADER = "CRYPT_RES\x0A\x00";

static constexpr size_t BYTES_TO_CIPHER = 256;

static constexpr uint8_t* getRawKey(size_t& keyLen)
{
keyLen = std::strlen(CIPHER_KEY);

return Input::getUtf8Bytes(CIPHER_KEY, keyLen);
}

};	

struct Encryptor : protected Common
{
static void encryptStream(ReadOnlyStream& input, WriteOnlyStream& output, ProgressCallback progress);
};

struct Decryptor : protected Common
{
static void decryptStream(ReadOnlyStream& input, WriteOnlyStream& output, ProgressCallback progress);
};

}

namespace Cdat = SexyCryptors::PopCapCipheredData;

using CdatEncryptor = Cdat::Encryptor;
using CdatDecryptor = Cdat::Decryptor;
	
extern "C"
{			  
DLL_EXPORT void Cdat_EncryptFile(char* input, char* output, ProgressCallback progress);
DLL_EXPORT void Cdat_DecryptFile(char* input, char* output, ProgressCallback progress);
}