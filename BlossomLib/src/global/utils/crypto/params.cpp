#include "params.h"

    size_t Crypto::getBlockLen(const uint8_t* data, size_t dataSize, size_t expectedLen)
    {
        return getBlockLen(data, dataSize, expectedLen, expectedLen);
    }
    
    size_t Crypto::getBlockLen(const uint8_t* data, size_t dataSize, size_t minLen, size_t maxLen) {
        if (dataSize < minLen) {
            return minLen;
        } else if (dataSize > maxLen) {
            return maxLen;
        }
        return dataSize;
    }
    
    void Crypto::checkBlock(uint8_t* &data, size_t &dataSize, size_t expectedSize) 
    {
        checkBlock(data, dataSize, expectedSize, expectedSize);
    }

    void Crypto::checkBlock(uint8_t* &data, size_t &dataSize, size_t minSize, size_t maxSize) 
    {
        if (dataSize < minSize) {
            std::fill(data + dataSize, data + minSize, 0);
            dataSize = minSize;
        } else if (dataSize > maxSize) {
            std::memset(data + maxSize, 0, dataSize - maxSize);
            dataSize = maxSize;
        }
    }

    uint8_t* Crypto::initVector(uint8_t* key, size_t keyLen, int expectedLen, int startIndex, 
                                int& outSize) 
    {
        return initVector(key, keyLen, expectedLen, expectedLen, startIndex, outSize);
    }

    uint8_t* Crypto::initVector(uint8_t* key, size_t keyLen, int minLen, int maxLen, int startIndex,
                                int& outSize) 
    {
        size_t vectorSize = getBlockLen(key, keyLen, minLen, maxLen);
        uint8_t* iv = static_cast<uint8_t*>(std::malloc(vectorSize) );

        startIndex = std::clamp(startIndex, 0, vectorSize - 1);

        for (size_t i = 0; i < vectorSize; ++i) {
            iv[i] = key[startIndex + i];
        }

        if (keyLen == vectorSize) {
            std::reverse(iv, iv + vectorSize);
        }

        *outSize = vectorSize;

        return iv;
    }

extern "C"
{
    size_t Crypto_GetBlockLen(const uint8_t* data, int64_t dataSize, int64_t expectedLen) 
    {
        return Crypto::getBlockLen(data, dataSize, expectedLen);
    }

    size_t Crypto_GetBlockLen(const uint8_t* data, int64_t dataSize, int64_t minLen, int64_t maxLen) 
    {
        return Crypto::getBlockLen(data, static_cast<size_t>(dataSize), minLen, maxLen);
    }

    void Crypto_CheckBlock(uint8_t* &data, int64_t &dataSize, int64_t minSize, int64_t maxSize) 
    {
        Crypto::checkBlock(data, dataSize, minSize, maxSize);
    }
    
    void Crypto_CheckBlock(uint8_t* &data, int64_t &dataSize, int64_t expectedSize) {
        Crypto::checkBlock(data, static_cast<size_t&>(dataSize), expectedSize);
    }

    uint8_t* Crypto_InitV(const uint8_t* key, int64_t keyLen, int expectedLen, int startIndex, int& outSize) 
    {
        return Crypto::initVector(key, keyLen, expectedLen, startIndex, outSize);
    }

    uint8_t* Crypto_InitV(const uint8_t* key, int64_t keyLen, int minLen, int maxLen, int startIndex, int& outSize) 
    {
        return Crypto::initVector(key, keyLen, minLen, maxLen, startIndex, outSize);
    }
}
