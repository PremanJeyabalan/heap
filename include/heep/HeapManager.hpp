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

    class HeapManager {
    public:
        explicit HeapManager(size_t bytes);

//        delete move and copy constructors
        HeapManager(const HeapManager&)=delete;
        HeapManager(HeapManager&&)=delete;

        ~HeapManager();

    public:
        template <class Finder>
        void* allocate(size_t bytes);
        void deallocate(void* ptr);

    public:
        void print() const;
        [[nodiscard]] size_t size() const;
        [[nodiscard]] size_t capacity() const;
        [[nodiscard]] size_t getFreeSize() const;

    private:
        void* reserve(size_t bytes);
        HeapBlock* expandHeap(size_t bytes);


    private:
        vmem::VirtualStaticHeap m_heap;
        size_t m_size;
        size_t m_capacity;
        void* m_memoryAddr;
        FreeList m_freeList;
    };

    HeapManager::HeapManager(size_t bytes) :
            m_heap{}, m_size{0}, m_capacity{0}, m_memoryAddr{reserve(helpers::get_aligned_16_bytes(bytes))}, m_freeList{m_memoryAddr, m_capacity}
    {}

    HeapManager::~HeapManager() {
        m_memoryAddr = nullptr;
        m_size = 0;
        m_capacity = 0;
    }

    template<class Finder>
    void* HeapManager::allocate(size_t bytes) {
        size_t alignedSize = helpers::get_aligned_block(bytes);

        std::optional<HeapBlock*> victimOpt = Finder::FindVictim(m_freeList, alignedSize);
        HeapBlock* victimBlock = victimOpt.has_value() ?
                victimOpt.value() : expandHeap(alignedSize);

        size_t startOffsetFromVictim = victimBlock->getSize() - alignedSize;
        void* resBlockAddr = helpers::get_block_advanced_by_size(victimBlock, startOffsetFromVictim);
        size_t newVictimSize = startOffsetFromVictim;
//        TODO: Resize logic between removing block and changing size
        victimBlock = m_freeList.resizeBlock(victimBlock, newVictimSize);

        auto* resBlock = HeapBlock::CreateFreeBlockAtMemory(resBlockAddr, false, nullptr, nullptr);
        m_size += alignedSize;
        return resBlock;
    }

//    TODO: Not implemented
    void HeapManager::deallocate(void *ptr) {

    }

    void HeapManager::print() const {
        fmt::print("--- Current Memory State ---\n");
        fmt::print("[STATS]");
        fmt::print("Size: {} Capacity: {} Free Space: {}\n", size(), capacity(), getFreeSize());
        fmt::print("[REGION {}]\n", m_memoryAddr);
        m_freeList.print();
    }

    size_t HeapManager::size() const {
        return m_size;
    }

    size_t HeapManager::capacity() const {
        return m_capacity;
    }

    size_t HeapManager::getFreeSize() const {
        return m_freeList.size();
    }

    void *HeapManager::reserve(size_t bytes) {
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
