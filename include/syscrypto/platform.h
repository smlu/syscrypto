#pragma once
#ifdef __APPLE__
#   include "TargetConditionals.h"
#elif defined(_WIN32)
#   define NOMINMAX 
#   include <windows.h>
#   undef NOMINMAX 
#endif

namespace syscrypto {
    enum class os
    {
        windows,
        macos,
        ios,
        linux,
        android,
        unknown_os
    };


#ifdef __APPLE__
#   ifdef TARGET_OS_MAC
#       define SC_PLATFORM_OS_MACOS 1
        inline constexpr auto platform_os = os::macos;
#   else
#       define SC_PLATFORM_OS_IOS 1
        inline constexpr auto platform_os = os::ios;
#   endif
#elif defined(_WIN32)
#   define SC_PLATFORM_OS_WINDOWS 1
    inline constexpr auto platform_os = os::windows;
#elif defined(__linux__)
#   ifdef __ANDROID__
#       define SC_PLATFORM_OS_ANDROID 1
        inline constexpr auto platform_os = os::android;
#   else
#       define SC_PLATFORM_OS_LINUX 1
        inline constexpr auto platform_os = os::linux;
#   endif
#else
#   warning "Unknown platform"
    inline constexpr auto platform_os = os::unknown_os;
#endif
}