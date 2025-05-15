#include "BlosomLib/src/modules/security/XXTea/common.h"
#include <memory>
#include <stdexcept>
#include <cstring>
#include <iostream>

namespace SexyCryptors::XXLua
{
// Internal defs

struct Common
{
private:
static const char* CIPHER_KEY = "7ec34b808tk94hf1";

protected:
static const char* HEADER = "XXTEA";

static const uint8_t* getRawKey(size_t &keyLen)
{
keyLen = std::strlen(CIPHER_KEY);

return Input::getUtf8Bytes(CIPHER_KEY, keyLen);
}

};

struct Encryptor : protected Common
{
static void encryptStream(ReadOnlyStream& input, WriteOnlyStream& output, ProgressCallback progress);

private:
static uint8_t* encryptData(uint8_t* input, size_t len, size_t& outLen, ProgressCallback progress);
};

struct Decryptor : protected Common
{
static void decryptStream(ReadOnlyStream& input, WriteOnlyStream& output, ProgressCallback progress);

private:
static uint8_t* decryptData(uint8_t* input, size_t len, size_t& outLen, ProgressCallback progress);
};

}

namespace XXLua = SexyCryptors::XXLua;

using XXLuaEncryptor = XXLua::Encryptor;
using XXLuaDecryptor = XXLua::Decryptor;
	
extern "C"
{				  
DLL_EXPORT void XXLua_EncryptFile(char* input, char* output, ProgressCallback progress);						  
DLL_EXPORT void XXLua_DecryptFile(char* input, char* output, ProgressCallback progress);	
}