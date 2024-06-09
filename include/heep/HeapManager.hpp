//
// Created by prema on 9/6/2024.
//

#ifndef HEEP_HEAPMANAGER_HPP
#define HEEP_HEAPMANAGER_HPP

#include <cstdlib>
#include <fmt/format.h>

#include "FreeList.hpp"
#include "Helpers.hpp"
#include "vmem/VirtualStaticHeap.hpp"

namespace heep {

    template <class Finder>
    class HeapManager {
    public:
        explicit HeapManager(size_t bytes);

//        delete move and copy constructors
        HeapManager(const HeapManager&)=delete;
        HeapManager(HeapManager&&)=delete;

        ~HeapManager();

    public:
        void* allocate(size_t bytes);
        void deallocate(void* ptr);

    public:
        void print() const;
        [[nodiscard]] size_t size() const;
        [[nodiscard]] size_t capacity() const;
        [[nodiscard]] size_t getFreeSize() const;

    private:
        void* reserve(size_t bytes);


    private:
        vmem::VirtualStaticHeap m_heap;
        size_t m_size;
        size_t m_capacity;
        void* m_memoryAddr;
        FreeList m_freeList;
    };

    template <class Finder>
    HeapManager<Finder>::HeapManager(size_t bytes) :
            m_heap{}, m_size{0}, m_capacity{0}, m_memoryAddr{reserve(helpers::get_aligned_16_bytes(bytes))}, m_freeList{m_memoryAddr, m_capacity}
    {}

    template<class Finder>
    HeapManager<Finder>::~HeapManager() {
        m_memoryAddr = nullptr;
        m_size = 0;
        m_capacity = 0;
    }


//    TODO: Not implemented
    template<class Finder>
    void *HeapManager<Finder>::allocate(size_t bytes) {
        return nullptr;
    }

//    TODO: Not implemented
    template<class Finder>
    void HeapManager<Finder>::deallocate(void *ptr) {

    }

    template<class Finder>
    void HeapManager<Finder>::print() const {
        fmt::print("--- Current Memory State ---\n");
        fmt::print("[STATS]");
        fmt::print("Size: {} Capacity: {} Free Space: {}\n", size(), capacity(), getFreeSize());
        fmt::print("[REGION {}]\n", m_memoryAddr);
        m_freeList.print();
    }

    template<class Finder>
    size_t HeapManager<Finder>::size() const {
        return m_size;
    }

    template<class Finder>
    size_t HeapManager<Finder>::capacity() const {
        return m_capacity;
    }

    template<class Finder>
    size_t HeapManager<Finder>::getFreeSize() const {
        return m_freeList.size();
    }

    template<class Finder>
    void *HeapManager<Finder>::reserve(size_t bytes) {
        void* top;
        if ((top = m_heap.request(bytes)) == (void*) -1) {
            fmt::print("[ERROR] Failed to allocate: {} bytes to the heap\n", bytes);
            print();
            throw;
        }

        m_capacity += bytes;
        return top;
    }
}

#endif //HEEP_HEAPMANAGER_HPP
