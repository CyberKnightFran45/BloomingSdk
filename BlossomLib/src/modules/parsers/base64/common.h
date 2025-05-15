#pragma once

#include "../../../global/lib_export.h"
#include "../../../global/callback.h"
#include "../../file_manager.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <cstdlib> 
#include <cstring>
#include <cstddef>

namespace BlossomLib::Modules::Parsers::Base64
{
// Internal defs

struct Common
{
protected:

static const char* ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char* ALPHABET_WEB = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
};

struct Encoder : protected Common
{
char* encodeData(const uint8_t* input, size_t len, bool webSafe, size_t* outLen, ProgressCallback progress);
void encodeStream(ReadOnlyStream& input, WriteOnlyStream& output, bool webSafe, int64_t maxBytes, ProgressCallback progress);
};

struct Decoder : protected Common
{
uint8_t* decodeData(const char* input, size_t len, bool webSafe, size_t* outLen, ProgressCallback progress);
void decodeStream(ReadOnlyStream& input, WriteOnlyStream& output, bool webSafe, int64_t maxBytes, ProgressCallback progress);

private:
static uint8_t reverse_table[256] = { 0 };

static bool reverse_initialized = false;

static void initReverseTable(const char* alphabet);
};

}

namespace base64 = BlossomLib::Modules::Parsers::Base64;

using Base64Encoder = base64::Encoder;
using Base64Decoder = base64::Decoder;

extern "C"
{
DLL_EXPORT char* Base64_EncodeData(const uint8_t* input, int64_t len, bool webSafe, int64_t* outLen);
DLL_EXPORT void Base64_EncodeFile(const char* input, const char* output, bool webSafe, int64_t maxBytes, ProgressCallback progress);

DLL_EXPORT char* Base64_DecodeData(const uint8_t* input, int64_t len, bool webSafe, int64_t* outLen);
DLL_EXPORT void Base64_DecodeFile(const char* input, const char* output, bool webSafe, int64_t maxBytes, ProgressCallback progress);
}