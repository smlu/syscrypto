#pragma once
#include "../../platform.h"

#include <iterator>
#include <limits>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace syscrypto::windows {

    constexpr DWORD to_dword(std::size_t n)
    {
        if (n > static_cast<std::size_t>((std::numeric_limits<DWORD>::max)())) {
            throw std::overflow_error("to_dword failed, 'n' is too big");
        }
        return static_cast<DWORD>(n);
    }

    template<typename Container, typename = std::void_t<
        decltype(std::size(std::declval<Container>())),
            decltype(std::data(std::declval<Container>())),
                typename Container::value_type>>
    DATA_BLOB make_data_blob(Container& d)
    {
        static_assert(std::is_trivial_v<typename Container::value_type>, "Container's value_type must be a trivial type!");
        return DATA_BLOB{ to_dword(d.size()), 
            reinterpret_cast<BYTE*>(const_cast<std::byte*>(d.data())) };
    }

    std::wstring str2wstr(const std::string_view str)
    {
        if (str.length() > static_cast<std::size_t>((std::numeric_limits<int>::max)()))
        {
            throw std::overflow_error(
                "str2wstr failed: str length is too big to convert to wstring"
            );
        }

        const int strlen = static_cast<int>(str.length());

        UINT cp = CP_UTF8;
        int rlen = MultiByteToWideChar(cp, MB_ERR_INVALID_CHARS, str.data(), strlen, nullptr, 0);
        if (!rlen)
        {
            cp = CP_ACP;
            int rlen = MultiByteToWideChar(cp, MB_ERR_INVALID_CHARS, str.data(), strlen, 0, 0);
            if (!rlen) {
                return L"";
            }
        }

        std::wstring result(rlen, L'0');
        result.resize(rlen);
        if (!MultiByteToWideChar(cp, 0, str.data(), strlen, result.data(), rlen)) {
            return L"";
        }

        return result;
    }

    //! Get all hwnds associated with current thread.
    std::vector<HWND> WINAPI get_current_thread_hwnds()
    {
        const auto dwCurThreadID = GetCurrentThreadId();
        std::vector<HWND> vhWnds;
        HWND hCurWnd = NULL;
        do
        {
            hCurWnd = FindWindowEx(NULL, hCurWnd, NULL, NULL);
            auto dwThreadId = GetWindowThreadProcessId(hCurWnd, NULL);
            if (dwThreadId == dwCurThreadID) {
                vhWnds.push_back(hCurWnd);
            }
        } while (hCurWnd != NULL);
        return vhWnds;
    }

    HWND WINAPI get_owner_hwnd(HWND hwnd)
    {
        if (auto h = GetWindow(hwnd, GW_OWNER)) {
            return h;
        }

        return hwnd;
    }

    //! Get top hwnd associated with current thread.
    HWND WINAPI get_current_thread_top_hwnd()
    {
        auto hwnds = get_current_thread_hwnds();
        if (hwnds.empty()) {
            return nullptr;
        }

        return get_owner_hwnd(*hwnds.rbegin());
    }

    //! Get top hwnd associated with current thread or console window hwnd.
    HWND WINAPI get_top_hwnd()
    {
        auto hwnd = get_owner_hwnd(GetActiveWindow());
        if (hwnd) {
            return hwnd;
        }

        hwnd = get_current_thread_top_hwnd();
        if (hwnd) {
            return hwnd;
        }

        return GetConsoleWindow();
    }
}