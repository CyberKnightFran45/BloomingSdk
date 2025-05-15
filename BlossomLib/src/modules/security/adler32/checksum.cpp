#include "common.h"

namespace adler32 
{
// Adler32 Checksum

uint32_t Checksum::calculate(const uint8_t* data, size_t len, ProgressCallback progress) 
{
    uint32_t a = 1, b = 0;
    size_t totalBlocks = (len + OPTIMAL_BLOCK_SIZE - 1) / OPTIMAL_BLOCK_SIZE;

    for (size_t blockIndex = 0; blockIndex < totalBlocks; ++blockIndex) {
        size_t remaining = len - blockIndex * OPTIMAL_BLOCK_SIZE;
        size_t blockSize = (remaining > OPTIMAL_BLOCK_SIZE) ? OPTIMAL_BLOCK_SIZE : remaining;

        const uint8_t* blockStart = data + blockIndex * OPTIMAL_BLOCK_SIZE;
        const uint8_t* blockEnd = blockStart + blockSize;

        for (const uint8_t* ptr = blockStart; ptr < blockEnd; ++ptr) {
            a += *ptr;
            b += a;
        }

        a %= MOD_ADLER;
        b %= MOD_ADLER;

        if (progress)
            progress(blockIndex, totalBlocks, "ACTION_CALCULATE_CHECKSUM", false);
        
    }

    return (b << 16) | a;
}

uint32_t Checksum::calculate(ReadOnlyStream& input, ProgressCallback progress)
{
return calculate(input.Data(), input.Size(), progress);
}

bool isIntegral(ReadOnlyStream& target, uint32_t expected)
{
uint32_t checksum = calculate(target, nullptr);

return checksum == expected;
}

}

extern "C" 
{

uint32_t Adler32_Calculate(const uint8_t* data, uint64_t len)
{
return Adler32Checksum::calculate(data, len, nullptr);
}

void Adler32_Digest(const char* input, const char* output, ProgressCallback progress)
{
auto inStream = fileMgr::openRead(input);
uint32_t checksum = Adler32Checksum::calculate(*inStream, progress);

auto outStream = fileMgr::openWrite(output);
outStream->WriteUInt32(checksum);
}

bool Adler32_Check(const char* source, const char* target)
{
auto srcStream = fileMgr::openRead(source);
uint32_t checksum = Adler32Checksum::calculate(*srcStream, nullptr);

auto targetStream = fileMgr::openRead(target);

return Adler32Checksum::isIntegral(*targetStream, checksum);
}

}