#pragma once
#include <string.h>
#include <memory>
#include <type_traits>

#include "platform.h"
#include "bytes_span.h"

namespace syscrypto {

    namespace detail {
#if defined(__STDC_LIB_EXT1__) && defined(__STDC_WANT_LIB_EXT1__) && __STDC_WANT_LIB_EXT1__
        inline constexpr bool has_stdc_lib_ext1 = true;
#else
        inline constexpr bool has_stdc_lib_ext1 = false;
#endif
        template<typename T>
        void zeromem(T v)
        {
            static_assert(std::is_same_v<T, bytes_span> || std::is_same_v<T, pd_bytes_span>,
                "T must bytes_span or pd_bytes_span type!");

            if constexpr (platform_os == os::windows) {
                RtlSecureZeroMemory(v.data(), v.size());
            }
            else if constexpr (detail::has_stdc_lib_ext1 || true) {
                memset_s(v.data(), v.size(), 0, v.size());
            }
            else
            {
                volatile std::byte* volatile  p = v.data();
                auto s = v.size();
                while (s-- > 0) {
                    *p++ = std::byte(0);
                }
            }
        }
    }

    inline void zeromem(bytes_span bv)
    {
        detail::zeromem(bv);
    }

    inline void zeromem(pd_bytes_span pdbv)
    {
        detail::zeromem(pdbv);
    }


    template<typename T, typename ... Rest>
    struct sec_allocator : std::allocator<T>
    {
        using std::allocator<T>::allocator;

#if __cplusplus <= 201703L
        template<class U> struct rebind { using other = sec_allocator<U>; };
#endif

        template <class U>
        sec_allocator(const sec_allocator<U>&) noexcept {}

        void deallocate(T* p, std::size_t n)
        {
            zeromem(bytes_span(reinterpret_cast<std::byte*>(p), n * sizeof(T)));
            std::allocator<T>::deallocate(p, n);
        }
    };
}