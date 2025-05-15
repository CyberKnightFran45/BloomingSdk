#pragma once

#include "../../../global/lib_export.h"
#include "../../../global/callback.h"
#include "../../file_manager.hpp"
#include <cstdint>
#include <vector>
#include <span>
#include <memory>

namespace BlossomLib::Modules::Security::Adler32
{
// Internal defs

struct Common
{
protected:
static constexpr uint32_t MOD_ADLER = 65521;

static constexpr size_t OPTIMAL_BLOCK_SIZE = 3800; 
};

struct Checksum : protected Common
{
static uint32_t calculate(const uint8_t* data, size_t len, ProgressCallback progress);
static uint32_t calculate(ReadOnlyStream& input, ProgressCallback progress);

static bool isIntegral(ReadOnlyStream& source, uint32_t expected);
};

}

namespace adler32 = BlossomLib::Modules::Security::Adler32;

using Adler32Checksum = Adler32::Checksum;

extern "C" 
{
DLL_EXPORT uint32_t Adler32_Calculate(const uint8_t* data, size_t len);

DLL_EXPORT void Adler32_Digest(const char* input, const char* output, ProgressCallback progress);

DLL_EXPORT bool Adler32_Check(const char* source, const char* target);
}