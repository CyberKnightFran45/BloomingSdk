#pragma once

#include "../../../global/lib_export.h"
#include "../../../global/callback.h"
#include "../../file_manager.hpp"
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <cstring>

namespace BlossomLib::Modules::Security::Xor
{
uint8_t* cipherData(const uint8_t* input, size_t len, const uint8_t* key, size_t keyLen, ProgressCallback progress);

void cipherStream(ReadOnlyStream& input, WriteOnlyStream& output, const uint8_t* key, size_t keyLen, int64_t maxBytes, ProgressCallback progress);
}

namespace xOr = BlossomLib::Modules::Security::Xor;

extern "C" 
{
DLL_EXPORT uint8_t* Xor_CipherData(const uint8_t* input, uint64_t len, const uint8_t* key, int keyLen);

DLL_EXPORT void Xor_CipherFile(char* input, char* output, const uint8_t* key, int keyLen, int64_t maxBytes, ProgressCallback progress);
}