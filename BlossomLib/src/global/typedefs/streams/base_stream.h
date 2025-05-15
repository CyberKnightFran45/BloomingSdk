#pragma once

#include "../endian.hpp"
#include <stdexcept>
#include <cstdint>
#include <vector>
#include <memory>
#include <cstring>
#include <system_error>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#endif

struct BaseStream
{

protected:
    const char* filePath;
    size_t fileSize;
    size_t currentPos;
    uint8_t* mappedData;
    bool isMemoryBuffer = false;
};