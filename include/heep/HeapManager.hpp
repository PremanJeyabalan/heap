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

    public:
        [[nodiscard]] HeapBlock* getFreeHead() const;
        [[nodiscard]] HeapBlock* getFreeTail() const;

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

    template<typename Finder>
    [[nodiscard]] void* HeapManager::allocate(size_t bytes) {
        size_t alignedSize = helpers::get_aligned_block(bytes);

        std::optional<HeapBlock*> victimOpt = Finder()(m_freeList, alignedSize);
        HeapBlock* victimBlock = victimOpt.has_value() ?
                victimOpt.value() : expandHeap(alignedSize);

        size_t newVictimSize = victimBlock->getSize() - alignedSize;
        std::optional<HeapBlock*> victimBlockResized = m_freeList.resizeBlock(victimBlock, newVictimSize);
        void* resBlockAddr = victimBlockResized.has_value() ?
                             victimBlockResized.value()->getEndAddr() : victimBlock;
        auto* resBlock = HeapBlock::CreateAllocBlockAtMemory(resBlockAddr, alignedSize);
        m_size += alignedSize;
        return resBlock->getDataAddr();
    }

}

#endif //HEEP_HEAPMANAGER_HPP
