#pragma once

#include "../../lib_export.h"
#include <cstring>
#include <cstdint>
#include <algorithm>
#include <cstdlib>

struct Crypto
{
static size_t getBlockLen(const uint8_t* data, size_t dataSize, size_t expectedLen);
static size_t getBlockLen(const uint8_t* data, size_t dataSize, size_t minLen, size_t maxLen);

static void checkBlock(uint8_t* &data, size_t &dataSize, size_t expectedSize);
static void checkBlock(uint8_t* &data, size_t &dataSize, size_t minSize, size_t maxSize);

static uint8_t* initVector(const uint8_t* key, size_t keyLen, int expectedLen, int startIndex, 
                           int& outSize);

static uint8_t* initVector(const uint8_t* key, size_t keyLen, int minLen, int maxLen, int startIndex,
                           int& outSize);

};

extern "C"
{
DLL_EXPORT size_t Crypto_GetBlockLen(const uint8_t* data, int64_t dataSize, int64_t expectedLen);	

DLL_EXPORT size_t Crypto_GetBlockLen(const uint8_t* data, int64_t dataSize, int64_t minLen, 
                                     int64_t maxLen);

DLL_EXPORT void Crypto_CheckBlock(uint8_t* &data, int64_t &dataSize, int64_t expectedSize);

DLL_EXPORT void Crypto_CheckBlock(uint8_t* &data, int64_t &dataSize, int64_t minSize, int64_t maxSize);

DLL_EXPORT uint8_t* Crypto_InitV(const uint8_t* key, int64_t keyLen, int expectedLen, int startIndex,
                                 int& outSize);

DLL_EXPORT uint8_t* Crypto_InitV(const uint8_t* key, int64_t keyLen, int minLen, int maxLen, 
                                 int startIndex, int& outSize);

}