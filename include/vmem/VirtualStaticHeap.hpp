//
// Created by prema on 9/6/2024.
//


#ifndef HEEP_VIRTUALSTATICHEAP_HPP
#define HEEP_VIRTUALSTATICHEAP_HPP

#include <cstdlib>
#include <cstdint>

namespace heep::vmem {
    static constexpr uint64_t page_size = 4096;
    static constexpr uint64_t max_heap_size = page_size * 1000;

    class VirtualStaticHeap {
    public:
        VirtualStaticHeap();

        //delete move and copy constructors
        VirtualStaticHeap(const VirtualStaticHeap&)=delete;
        VirtualStaticHeap(VirtualStaticHeap&&)=delete;

        VirtualStaticHeap& operator=(const VirtualStaticHeap&)=delete;
        VirtualStaticHeap& operator=(VirtualStaticHeap&&)=delete;

        ~VirtualStaticHeap();

    public:
        void* request(size_t increment);

    private:
        char* m_heapAddr;
        char* m_brkAddr;
        char* m_maxBrkAddr;
    };

}


#endif //HEEP_VIRTUALSTATICHEAP_HPP
