#pragma once

#include "BlossomLib/src/global/lib_export.hpp"
#include <cstdint>

namespace SexyConverter::IntegersGuard
{
static constexpr uint32_t CIPHER_KEY = 13;
static constexpr uint32_t LOGIC_FACTOR = 0x1F;
static constexpr uint32_t EXPECTED_BASE = 32;
static constexpr uint32_t BITMASK = 0xFF;

    int32_t encrypt(const uint32_t target);
	
    int32_t decrypt(const uint32_t target);
}

namespace intGuard = SexyConverter::IntegersGuard;

extern "C"
{
DLL_EXPORT int32_t IntGuard_Encrypt(uint32_t target);

DLL_EXPORT int32_t IntGuard_Decrypt(uint32_t target);
}