//
// Created by prema on 9/6/2024.
//

#ifndef HEEP_FLIST_HPP
#define HEEP_FLIST_HPP

#include "HeapBlock.hpp"

namespace heep {
    class FreeList {
    public:
        FreeList(void* memory, size_t capacity);

    public:
        HeapBlock* pushBack(void* memory, size_t size);
        HeapBlock* pushFront(void* memory, size_t size);
        void popBack();
        void popFront();
        void erase(HeapBlock* block);
        HeapBlock* insert(void* memory, size_t size, const void* start, const void* end);

    public:
        void print() const;

    public:
        [[nodiscard]] HeapBlock* getHead() const;
        [[nodiscard]] HeapBlock* getTail() const;
        [[nodiscard]] size_t size() const;

    private:
        [[nodiscard]] HeapBlock* findNextFree(HeapBlock* block, const void* end) const;

    private:
        HeapBlock* m_head;
        HeapBlock* m_tail;
        size_t m_size;
    };
}

#endif //HEEP_FLIST_HPP
