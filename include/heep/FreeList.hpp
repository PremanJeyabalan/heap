//
// Created by prema on 9/6/2024.
//

#ifndef HEEP_FLIST_HPP
#define HEEP_FLIST_HPP

#include <optional>
#include "HeapBlock.hpp"

namespace heep {
    class FreeList {
    public:
        FreeList(void* memory, size_t capacity);

    public:
        HeapBlock* pushBack(void* memory, size_t size);
        HeapBlock* pushFront(void* memory, size_t size);
        HeapBlock* insert(void* memory, size_t size, const void* start, const void* end);
        HeapBlock* resizeBlock(void* memory, size_t newSize);
        void popBack();
        void popFront();
        void erase(HeapBlock* block);

    public:
        void print() const;

    public:
        [[nodiscard]] HeapBlock* getHead() const;
        [[nodiscard]] HeapBlock* getTail() const;
        [[nodiscard]] size_t size() const;

    private:
        std::pair<HeapBlock*, HeapBlock*> findPrevAndNextFree(HeapBlock* block) const;

    private:
        HeapBlock* m_head;
        HeapBlock* m_tail;
        size_t m_size;
    };
}

#endif //HEEP_FLIST_HPP
