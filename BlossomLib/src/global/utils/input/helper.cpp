#include "helper.h"

char* Input::convertHexStr(const uint8_t* input, size_t len, StringCase strCase, char separator) 
{
	
    if (!input || len == 0) 
		return nullptr;

    size_t outputLen = (separator != 0) ? len * 2 + (len - 1) : len * 2;
    auto output = static_cast<char*>(std::malloc(outputLen + 1) );

    size_t index = 0;
    const char* hexDigits = (strCase == StringCase::Upper)
        ? "0123456789ABCDEF"
        : "0123456789abcdef";

    for (size_t i = 0; i < len; ++i) {
        unsigned char byte = input[i];
        output[index++] = hexDigits[(byte >> 4) & 0xF];
        output[index++] = hexDigits[byte & 0xF];

        if (separator != 0 && i < len - 1) {
            output[index++] = separator;
        }
    }

    output[outputLen] = '\0';
    return output;
}

uint8_t* Input::convertHexBytes(const char* src, size_t& byteCount, char separator) 
{
	
    if (!src || !byteCount)
		return nullptr;
    
    size_t len = strlen(src);
    auto bytes = static_cast<uint8_t*>(std::malloc(len / 2) );
	
    byteCount = 0;

    size_t i = 0;
    while (i < len) {
        while (i < len && src[i] == separator) ++i;

        if (i + 1 >= len) break;

        auto hexToByte = [](char ch) -> uint8_t {
            if (ch >= '0' && ch <= '9') return ch - '0';
            if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
            if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
            return 0;
        };

        uint8_t high = hexToByte(src[i]);
        uint8_t low = hexToByte(src[i + 1]);
        bytes[(*byteCount)++] = (high << 4) | low;

        i += 2;
    }

    return bytes;
}

uint8_t* Input::getUtf8Bytes(const char* str, size_t &byteCount) 
{
	
	if(!str)
	return nullptr;
	
        size_t len = strlen(str);
        byteCount = len;

        auto buffer = static_cast<uint8_t*>(std::malloc(len) );
        std::memcpy(buffer, str, len);
		
        return buffer;
    }

void Input::applyStrCase(char* target, StringCase strCase) 
{
	
    if (!target)
		return;

    for (size_t i = 0; target[i] != '\0'; ++i) {
        target[i] = (strCase == StringCase::Upper)
            ? static_cast<char>(std::toupper(static_cast<unsigned char>(target[i])))
            : static_cast<char>(std::tolower(static_cast<unsigned char>(target[i])));
    }
}

void Input::filterDate(const char* input, struct tm* outDate) {
    std::istringstream stream(input);
    stream >> std::get_time(outDate, "%Y-%m-%d %H:%M:%S");

    if (stream.fail()) {
        std::time_t now = std::time(nullptr);
        *outDate = *std::localtime(&now);
    }
}

double Input::filterNumber(const char* input) 
{
    std::regex pattern(R"(([-+]?\d*\.?\d+))");
    std::cmatch match;

    if (std::regex_search(input, match, pattern)) {
        return std::stod(match.str(1));
    }

    return 0.0;
}

bool Input::checkPadding(size_t size, size_t unit)
{
	return (size % unit) == 0;
}

size_t Input::getPaddedLen(size_t len, size_t unit)
{
	return checkPadding(len, unit) ? len : ( (len / unit + 1) * unit);
}

size_t Input::getUnpaddedLen(size_t len, size_t unit)
{
	return getPaddedLen(len, unit) - len;
}

inline std::string Input::formatSize(size_t bytes) 
{
    
	if (bytes < constants::ONE_KILOBYTE)
        return fmt::format("{} B", bytes);
	
    else if (bytes < constants::ONE_MEGABYTE)
        return fmt::format("{} KB", bytes / constants::ONE_KILOBYTE);
	
    else if (bytes < constants::ONE_GIGABYTE)
        return fmt::format("{} MB", bytes / constants::ONE_MEGABYTE);

return fmt::format("{} GB", bytes / constants::ONE_GIGABYTE);
}

extern "C" 
{

void Input_ApplyCase(char* target, int strCase) {
    Input::applyStrCase(target, strCase);
}

char* Input_ToHexStr(const uint8_t* input, int64_t len, int strCase, char separator) 
{
    return Input::convertHexStr(input, static_cast<size_t>(len), strCase, separator);
}

uint8_t* Input_ToHexBytes(const char* hexString, int64_t& byteCount, char separator)
{
size_t sCount = static_cast<size_t>(byteCount);
auto result = Input::convertHexBytes(hexString, &sCount, separator);

byteCount = static_cast<int64_t>(sCount);

return result;
}

double Input_ParseNumber(const char* input) {
    return Input::filterNumber(input);
}

}