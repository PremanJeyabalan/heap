//
// Created by prema on 9/6/2024.
//

#ifndef HEEP_HEAPBLOCK_HPP
#define HEEP_HEAPBLOCK_HPP

#include <cstdint>
#include <cstdlib>
#include <optional>

namespace heep {
    struct HeapBlock {
    public:
        HeapBlock(uint32_t size, bool free, HeapBlock *next, HeapBlock *prev);
        static HeapBlock* CreateFreeBlockAtMemory(void* memory, size_t size, HeapBlock* prev, HeapBlock* next);
        static HeapBlock* CreateAllocBlockAtMemory(void* memory, size_t size);
        static std::pair<
                std::optional<HeapBlock*>, std::optional<HeapBlock*>
        > GetNeighbours(HeapBlock* block, size_t size, const void* startBoundary, const void* endBoundary);

    public:
        void init(uint32_t pack, HeapBlock* next, HeapBlock* prev);
        void expand(size_t increment);
        void assign(HeapBlock* other);
        void print() const;

    public:
        [[nodiscard]] uint32_t getSize() const;
        [[nodiscard]] bool getFree() const;
        [[nodiscard]] HeapBlock* getNext() const;
        [[nodiscard]] HeapBlock* getPrev() const;

    public:
        [[nodiscard]] void* getEndAddr() const;
        [[nodiscard]] void* getDataAddr() const;
        [[nodiscard]] HeapBlock* getFooterAddr() const;
        [[nodiscard]] HeapBlock* getPrevStartAddr() const;

    public:
        void setSize(uint32_t size);
        void setFree(bool free);
        void setPrev(HeapBlock*);
        void setNext(HeapBlock*);
        HeapBlock** next();
        HeapBlock** prev();
        void setPack(uint32_t size, bool pack);
        void setFooter();

    private:
        uint32_t m_sizeFreePack;
        HeapBlock* m_next;
        HeapBlock* m_prev;
    };
}

#endif //HEEP_HEAPBLOCK_HPP
