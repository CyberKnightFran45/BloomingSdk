#pragma once

#include "../../lib_export.h"
#include "../../constants.h"
#include "../../../plugins/fmt/core.h"
#include <string>
#include <vector>
#include <cctype>
#include <ctime>
#include <regex>
#include <sstream>
#include <iomanip>
#include <random>
#include <cstring>

enum StringCase {
    Lower = 0,
    Upper = 1
};

struct Input
{
private:

public:
static void applyStrCase(char* target, StringCase strCase);

static char* convertHexStr(const uint8_t* input, size_t len, StringCase strCase, char separator);
static uint8_t* convertHexBytes(const char* src, size_t& byteCount, char separator);

static uint8_t* getUtf8Bytes(const char* str, size_t& byteCount);

static bool checkPadding(size_t size, size_t unit);
static size_t getPaddedLen(size_t len, size_t unit);
static size_t getUnpaddedLen(size_t len, size_t unit);

static std::string formatSize(size_t bytes);

static void filterDate(const char* input, struct tm* outDate);
static double filterNumber(const char* input);
};

extern "C" 
{
DLL_EXPORT void Input_ApplyCase(char* target, int strCase);

DLL_EXPORT char* Input_ToHexStr(const uint8_t* input, int64_t len, int strCase, char separator = ' ');
DLL_EXPORT uint8_t* Input_ToHexBytes(const char* hexString, int64_t& byteCount, char separator = ' ');

DLL_EXPORT double Input_ParseNumber(const char* input);
}