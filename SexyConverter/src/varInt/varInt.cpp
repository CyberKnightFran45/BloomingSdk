#include "varInt.hpp"

namespace varInt
{

int32_t toInt(const int32_t target, const bool isSigned)
{
auto buffer = std::make_unique<BinaryStream>(sizeof(int32_t) );

if(isSigned)
{
buffer->WriteUInt32(static_cast<uint32_t>(target) );
buffer->Seek(0);

return static_cast<int32_t>(buffer->ReadUVarInt() );
}

buffer->WriteInt32(target);
buffer->Seek(0);

return buffer->ReadVarInt();
}

int32_t fromInt(const int32_t source, const bool isSigned)
{
auto buffer = std::make_unique<BinaryStream>(sizeof(int32_t) );

if(isSigned)
{
buffer->WriteUVarInt(static_cast<uint32_t>(target) );
buffer->Seek(0);

return static_cast<int32_t>(buffer->ReadUInt32() );
}

buffer->WriteVarInt(target);
buffer->Seek(0);

return buffer->ReadInt32();
}

}

extern "C"
{

DLL_EXPORT int32_t VarIntToInt(int32_t target, bool isSigned)
{
return varInt::toInt(target, isSigned);
}

DLL_EXPORT int32_t IntToVarInt(int32_t target, bool isSigned)
{
return varInt::fromInt(target, isSigned);
}

}
