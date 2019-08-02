#pragma once
#include "skc.h"

namespace syscrypto {

    constexpr std::size_t SYSCRYPTO_VERSION = 0'01'00;
    constexpr std::size_t SYSCRYPTO_VERSION_MAJOR = SYSCRYPTO_VERSION / 10000;
    constexpr std::size_t SYSCRYPTO_VERSION_MINOR = SYSCRYPTO_VERSION / 100 % 100;
    constexpr std::size_t SYSCRYPTO_VERSION_PATCH = SYSCRYPTO_VERSION % 100;
}