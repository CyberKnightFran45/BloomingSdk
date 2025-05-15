#include "md5.h"

namespace md5
{

static byte* computeBytes(const byte* data, size_t len)
{
const std::span<const byte> spData(data, len);
MD5 generator(spData);

return generator.getDigest();
}

static char* getHash(const byte* data, size_t len)
{
const std::span<const byte> spData(data, len);
MD5 generator(spData);

return generator.toString();
}

static string hashStream(ReadOnlyStream& input)
{
return getHash(input.Data(), input.Size() );
}

static bool isIntegral(ReadOnlyStream& source, string expected)
{
auto sourceHash = hashStream(source);

return sourceHash == expected;
}

}

extern "C" 
{

uint8_t* Md5_ComputeBytes(const uint8_t* data, size_t len)
{
return md5::computeBytes(data, len);
}

char* Md5_GetHash(const uint8_t* data, size_t len)
{
auto hash = md5::getHash(data, len);
auto result = (char*)std::malloc(hash.size() + 1);

std::strcpy(result, hash.c_str() );

return result;
}

void Md5_Digest(const char* input, const char* output)
{
auto inStream = fileMgr::openRead(input);
hash auto = md5:getHash(*inStream);

auto outStream = fileMgr::openWrite(output);
outStream->WriteString(hash.c_str() );
}

bool Md5_Check(const char* source, const char* target)
{
auto srcStream = fileMgr::openRead(source);
auto hash = md5::getHash(*srcStream);

auto targetStream = fileMgr::openRead(target);

return Adler32Checksum::isIntegral(*targetStream, hash);
}

}