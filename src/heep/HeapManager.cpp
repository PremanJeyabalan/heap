//
// Created by prema on 22/6/2024.
//
#include "heep/HeapManager.hpp"

namespace heep {

    HeapManager::HeapManager(size_t bytes) :
            m_heap{}, m_size{0}, m_capacity{0}, m_memoryAddr{reserve(helpers::get_aligned_16_bytes(bytes))}, m_freeList{m_memoryAddr, m_capacity}
    {}

    HeapManager::~HeapManager() {
        m_memoryAddr = nullptr;
        m_size = 0;
        m_capacity = 0;
    }

    void HeapManager::deallocate(void *ptr) {
        void* startAddr = helpers::get_block_start_address_from_data(ptr);
//        fmt::print("Dealloc for start {}\n", startAddr);
        auto* block = static_cast<HeapBlock*>(startAddr);
        block->setFree(true);
//        fmt::print("Dealloc for block {}\n", (void*)block);
        m_freeList.insert(block, block->getSize(), m_memoryAddr, helpers::get_block_advanced_by_size(m_memoryAddr, m_capacity));
        m_size -= block->getSize();
    }

    void HeapManager::print() const {
        fmt::print("--- Current Memory State ---\n");
        fmt::print("[STATS]");
        fmt::print("Size: {} Capacity: {} Free Space: {}\n", size(), capacity(), getFreeSize());
        fmt::print("[REGION {}]\n", m_memoryAddr);
        auto* curr = static_cast<HeapBlock*>(m_memoryAddr);
        void* end = helpers::get_block_advanced_by_size(curr, m_capacity);
        while (curr < end) {
            curr->print();
            curr = static_cast<HeapBlock *>(curr->getEndAddr());
        }
        m_freeList.print();
        fmt::print("-------------------------------\n");
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

    HeapBlock* HeapManager::expandHeap(size_t bytes) {
        size_t incrCapacity = helpers::get_aligned_page(bytes);
        void* newFreeBlock = reserve(incrCapacity);

//        fmt::print("Expanding heap with new block: {}\n", (void*)newFreeBlock);

        HeapBlock* fListTail = m_freeList.getTail();
        if (fListTail == nullptr || fListTail->getEndAddr() != newFreeBlock) {
//            fmt::print("Expanded with no coalesce...: {}\n", (void*)newFreeBlock);
            return m_freeList.pushBack(newFreeBlock, incrCapacity);
        } else {

//            fmt::print("Expanded with coalesce...: {}\n", (void*)newFreeBlock);

            return m_freeList
                .resizeBlock(fListTail, fListTail->getSize() + incrCapacity)
                .value();
        }
    }

    HeapBlock *HeapManager::getFreeHead() const {
        return m_freeList.getHead();
    }

    HeapBlock *HeapManager::getFreeTail() const {
        return m_freeList.getTail();
    }
}