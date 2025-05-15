#include "intGuard.hpp"

namespace intGuard 
{

inline int32_t encrypt(const uint32_t target) 
{
    return static_cast<int32_t>(
        ((target ^ CIPHER_KEY) << (CIPHER_KEY & LOGIC_FACTOR)) |
        ((target ^ CIPHER_KEY) >> ((EXPECTED_BASE - (CIPHER_KEY & LOGIC_FACTOR)) & BITMASK))
    );
}

inline int32_t decrypt(const uint32_t target) 
{

    return CIPHER_KEY ^ static_cast<int32_t>(
        ((target >> (CIPHER_KEY & LOGIC_FACTOR)) |
         (target << ((EXPECTED_BASE - (CIPHER_KEY & LOGIC_FACTOR)) & BITMASK)))
    );
}

}

extern "C"
{
int32_t IntGuard_Encrypt(uint32_t target) { return intGuard::encrypt(target); }

int32_t IntGuard_Decrypt(uint32_t target) { return intGuard::decrypt(target); }
}