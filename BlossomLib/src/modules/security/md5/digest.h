#pragma once

#include "../../../global/lib_export.h"
#include "../../../plugins/md5/md5.h"
#include "../../file_manager.hpp"
#include <cstdint>
#include <memory>

namespace BlossomLib::Modules::Security::Md5
{
static byte* computeBytes(const byte* data, size_t len);
static string getHash(const byte* data, size_t len);

static string hashStream(ReadOnlyStream& input);
static bool isIntegral(ReadOnlyStream& source, string expected);
}

namespace md5 = BlossomLib::Modules::Security::Md5;

extern "C" 
{
DLL_EXPORT uint8_t* Md5_ComputeBytes(const uint8_t* data, size_t len);
DLL_EXPORT char* Md5_GetHash(const uint8_t* data, size_t len);

DLL_EXPORT void Md5_Digest(const char* input, const char* output);
DLL_EXPORT bool Md5_Check(const char* source, const char* target);
}