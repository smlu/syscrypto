#pragma once
#include <array>
#include <cstddef>
#include <vector>
#include "sec_allocator.h"

namespace syscrypto {

    struct bytes : std::vector<std::byte, std::allocator<std::byte>> {
        using std::vector<std::byte, std::allocator<std::byte>>::vector;
    };

    struct pd_bytes final : bytes {
        using bytes::bytes;
    };

    struct sec_bytes final : std::vector<std::byte, sec_allocator<std::byte>> {
        using std::vector<std::byte, sec_allocator<std::byte>>::vector;
    };

    template<std::size_t N>
    struct fixed_bytes final : std::array<std::byte, N> {};

    template<std::size_t N>
    struct pd_fixed_bytes final : std::array<std::byte, N> {};

    template<std::size_t N>
    struct sec_fixed_bytes final : std::array<std::byte, N>
    {
        ~sec_fixed_bytes()
        {
            zeromem(bytes_span(data(), size()));
        }
    };
}