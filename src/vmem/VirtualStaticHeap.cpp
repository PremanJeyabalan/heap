//
// Created by prema on 9/6/2024.
//

#include <cstdint>
#include "VirtualStaticHeap.hpp"


namespace heep::vmem {

    VirtualStaticHeap::VirtualStaticHeap() : m_heapAddr((char*) malloc(max_heap_size)) {
        m_brkAddr = m_heapAddr;
        m_maxBrkAddr = m_heapAddr + max_heap_size;
    }

    VirtualStaticHeap::~VirtualStaticHeap() {
        free(m_heapAddr);
        m_heapAddr = nullptr;
        m_maxBrkAddr = nullptr;
        m_brkAddr = nullptr;
    }

    void *VirtualStaticHeap::request(size_t increment) {
        char* oldBrk = m_brkAddr;

        if ((m_brkAddr + increment) > m_maxBrkAddr)
            return (void*) -1;

        m_brkAddr += increment;
        return (void*) oldBrk;
    }
}

