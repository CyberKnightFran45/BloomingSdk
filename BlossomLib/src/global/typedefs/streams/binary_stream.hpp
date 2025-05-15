#include "readonly_stream.h"
#include "writeonly_stream.h"

struct BinaryStream : public ReadOnlyStream, public WriteOnlyStream
{
// Ctors

BinaryStream(size_t bufferSize) : ReadOnlyStream(bufferSize), WriteOnlyStream(bufferSize) {}

BinaryStream(const uint8_t* data, size_t size) : ReadOnlyStream(data, size),  WriteOnlyStream(data, size) {}

// Methods

static auto OpenRead(const char* filePath)
{
return std::make_unique<ReadOnlyStream>(filePath);
}

static auto OpenWrite(const char* filePath)
{
return std::make_unique<WriteOnlyStream>(filePath);
}

}