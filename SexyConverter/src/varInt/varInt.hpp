#pragma once

#include "BlossomLib/src/global/lib_export.h"
#include "BlossomLib/src/global/typedefs/streams/binary_stream.hpp"

namespace SexyConverter::VariantLenInt
{
int toInt(const int32_t target, const bool isSigned);

int fromInt(const int32_t source, const bool isSigned);
}

namespace varInt = SexyConverter::VariantLenInt;

extern "C"
{
DLL_EXPORT int32_t VarIntToInt(int32_t target, bool isSigned);

DLL_EXPORT int32_t IntToVarInt(int32_t target, bool isSigned);
}