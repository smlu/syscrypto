#pragma once
#include <functional>
#include <type_traits>
#include "../platform.h"

#ifndef SC_PLATFORM_OS_WINDOWS
#   include <unistd.h>
#endif

#define sc_scope_exit(...) \
    const auto SC_ANONY_VAR(scse_fo_)##_ = syscrypto::utils::at_scope_exit_do(__VA_ARGS__)

#if defined(__COUNTER__)
# define SC_ANONY_VAR(prefix) SC_CAT(prefix, __COUNTER__)
#else
# define SC_ANONY_VAR(prefix) SC_CAT(prefix, __LINE__)
#endif 

#define SC_CAT_(s1, s2) s1##s2
#define SC_CAT(s1, s2) SC_CAT_(s1, s2)


namespace syscrypto::utils {

    template<typename Lambda>
    [[nodiscard]]
    static auto at_scope_exit_do(Lambda&& f)
    {
        struct scse_f_ final {
            scse_f_(Lambda&& f) : f_(std::move(f)) {}
            ~scse_f_() {
                f_();
            }
        private:
            std::function<void()> f_;
        };

        return scse_f_(std::forward<Lambda>(f));
    }

    /** Check if integer is a power of 2. */
    template <typename I>
    static inline constexpr bool is_pow2(I i) noexcept 
    {
        static_assert(std::is_integral_v<I>);
        return i > 0 && (i & (i - 1)) == 0;
    }

    static inline std::size_t get_page_size() noexcept
    {
#ifdef  SC_PLATFORM_OS_WINDOWS
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        return sysInfo.dwPageSize;
#else
        return sysconf(_SC_PAGESIZE);
#endif
    }
}