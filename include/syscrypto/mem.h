#pragma once
#include "bytes.h"
#include "bytes_span.h"

namespace syscrypto {

   /* struct pdm {

    };

    template<typename T>
    bool mem_encrypt(pd_bytes_span);
    bool mem_decrypt();*/

    /**
    * Locks the specified region of the process's virtual address space into physical memory, 
    * preventing that memory from being paged out to secondary storage.
    *
    * @param Pointer to the base address of region of pages 
    */
    void static mem_lock(void* addr, std::size_t size);
    void static mem_unlock(void* addr, std::size_t size);

}

#include "impl/mem.impl"