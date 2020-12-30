#pragma once
#include <cstdint>
#include <sys/mman.h>
#include "../utils.h"

namespace syscrypto {
    void mem_lock(void* addr, std::size_t size)
    {
        const std::size_t page_size   = utils::get_page_size();
        const std::size_t page_offset = static_cast<std::uintptr_t>(addr) % page_size;

        addr -= page_offset;  /* Adjust addr to page boundary */
        size += page_offset;  /* Adjust size with page_offset */

        if (mlock(addr, size)) {
            // TODO: throw system error
        }
    }

    void mem_unlock(void* addr, std::size_t size)
    {
        const std::size_t page_size   = utils::get_page_size();
        const std::size_t page_offset = static_cast<std::uintptr_t>(addr) % page_size;

        addr -= page_offset;  /* Adjust addr to page boundary */
        size += page_offset;  /* Adjust size with page_offset */

        if (munlock(addr, size)) {
            // TODO: throw system error
        }
    }
}