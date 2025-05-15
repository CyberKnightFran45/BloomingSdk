#pragma once

/**
 * @file endian.hpp
 * @brief Endianness types and utilities
 */

#include <cstdint>
#include <type_traits>

/**
 * @enum Endian
 * @brief Specifies byte order (endianness)
 */
enum class Endianness {
    LittleEndian, ///< Little-endian byte order (LSB first)
    BigEndian     ///< Big-endian byte order (MSB first)
};

/**
 * @brief Compile-time system endianness detection
 */
inline bool SystemIsLittleEndian()
{
    uint16_t test = 0x0001;
    return *reinterpret_cast<const uint8_t*>(&test) == 0x01;
}


/**
 * @brief Endianness conversion utilities
 */
#ifdef _WIN32
#define BYTE_SWAP_16 _byteswap_ushort
#define BYTE_SWAP_32 _byteswap_ulong
#define BYTE_SWAP_64 _byteswap_uint64
#else
#define BYTE_SWAP_16 __builtin_bswap16
#define BYTE_SWAP_32 __builtin_bswap32
#define BYTE_SWAP_64 __builtin_bswap64
#endif

/**
 * @brief Convert value between endianness formats
 * @tparam T Integral type to convert
 * @param value Input value to convert
 * @param endian Target endianness
 * @return Converted value if needed
 */
template<typename T>
inline T ConvertEndian(T value, Endianness endian) {
    static_assert(std::is_integral_v<T>, "Endian conversion only supports integral types");
    
    const bool needsSwap = (endian == Endianness::LittleEndian) != SystemIsLittleEndian;
    if (needsSwap) {
        if constexpr (sizeof(T) == 2) {
            return BYTE_SWAP_16(value);
        } else if constexpr (sizeof(T) == 4) {
            return BYTE_SWAP_32(value);
        } else if constexpr (sizeof(T) == 8) {
            return BYTE_SWAP_64(value);
        }
    }
    return value;
}

/**
 * @brief Check if system uses little-endian
 * @return True if system is little-endian
 */
inline bool IsLittleEndianSystem() {
    return SystemIsLittleEndian;
}

/**
 * @brief Check if system uses big-endian
 * @return True if system is big-endian
 */
inline bool IsBigEndianSystem() {
    return !SystemIsLittleEndian;
}