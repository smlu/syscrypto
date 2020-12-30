#pragma once
#include "../platform.h"
namespace syscrypto {
    void mem_lock(void* addr, std::size_t size)
    {
        HANDLE hndl = GetCurrentProcess();

        // Increase minimum and maximum working set sizes
        SIZE_T min, max;
        if (!GetProcessWorkingSetSize(hndl, &min, &max)) {
            //throw system_error(get_last_system_error_text());
        }

        if (!SetProcessWorkingSetSize(hndl, min + size, max + size)) {
            //throw system_error(get_last_system_error_text());
        }

        // Lock memory
        if (!VirtualLock((LPVOID)addr, size)) {
            //throw system_error(get_last_system_error_text());
        }
    }

    void mem_unlock(void* addr, std::size_t size)
    {
        HANDLE hndl = GetCurrentProcess();

        // Decrease minimum and maximum working set sizes
        SIZE_T min, max;
        if (!GetProcessWorkingSetSize(hndl, &min, &max)) {
            //throw system_error(get_last_system_error_text());
        }

        if (!SetProcessWorkingSetSize(hndl, min - size, max - size)) {
            //throw system_error(get_last_system_error_text());
        }

        // Unlock memory
        if (!VirtualUnlock((LPVOID)addr, size)) {
            //throw system_error(get_last_system_error_text());
        }
    }
}