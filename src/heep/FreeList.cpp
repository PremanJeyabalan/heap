//
// Created by prema on 9/6/2024.
//

#include <cstdlib>
#include <fmt/format.h>

#include "heep/FreeList.hpp"
#include "heep/Helpers.hpp"

namespace heep {
    FreeList::FreeList(void *memory, size_t size) :
    m_head{static_cast<HeapBlock*>(memory)}, m_tail{m_head}, m_size{size}
    {
        m_head->init(helpers::size_and_free_to_pack(size, true), nullptr, nullptr);
    }

    HeapBlock *FreeList::pushBack(void *memory, size_t size) {
        auto* block = static_cast<HeapBlock*>(memory);
        block->init(helpers::size_and_free_to_pack(size, true), nullptr, m_tail);
        if (m_tail != nullptr)
            *(m_tail->next()) = block;
        else
            m_head = block;

        m_tail = block;
        m_size += size;
        return m_tail;
    }

    HeapBlock *FreeList::pushFront(void *memory, size_t size) {
        auto* block = static_cast<HeapBlock*>(memory);
        block->init(helpers::size_and_free_to_pack(size, true), m_head, nullptr);
        if (m_head != nullptr)
            *(m_head->prev()) = block;

        m_head = block;
        m_size += size;
        return block;
    }

    void FreeList::popBack() {
        if (m_tail == nullptr)
            return;

        m_size -= m_tail->getSize();
        m_tail = m_tail->getPrev();
        if (m_tail != nullptr)
            *(m_tail->next()) = nullptr;
        else
            m_head = nullptr;
    }

    void FreeList::popFront() {
        if (m_head == nullptr)
            return;

        m_size -= m_head->getSize();
        m_head = m_head->getNext();
        if (m_head != nullptr)
            *(m_head->prev()) = nullptr;
        else
            m_tail = nullptr;
    }

    void FreeList::erase(HeapBlock *block) {
        if (m_head == nullptr)
            return;

        if (block == m_head)
            popFront();
        else if (block == m_tail)
            popBack();
        else {
            auto* prev = block->getPrev();
            auto* next = block->getNext();
            m_size -= block->getSize();

            *(prev->next()) = next;
            *(next->prev()) = prev;
        }
    }

    HeapBlock* FreeList::insert(void* memory, size_t size, const void* start, const void* end) {
        //handle empty free list
        if (m_head == nullptr)
            return pushBack(memory, size);

        m_size += size;
        auto* entry = static_cast<HeapBlock*>(memory);
        auto [entryPrev, entryNext] = getNeighbours(entry);
        bool coalesceRight = entryNext.has_value() && entryNext.value()->getFree();
        bool coalesceLeft = entryPrev.has_value() && entryPrev.value()->getFree();

        auto* newBlockStart = entry;
        size_t newBlockSize = size;
        HeapBlock* newBlockNext = nullptr;
        HeapBlock* newBlockPrev = nullptr;

        if (!coalesceRight && !coalesceLeft) {
            auto [prev, next] = findPrevAndNextFree(newBlockStart);
            newBlockPrev = prev;
            if (newBlockPrev)
                *(prev->next()) = newBlockStart;

            newBlockNext = next;
            if (newBlockNext)
                *(next->prev()) = newBlockStart;

        } else if (coalesceLeft) {
            auto entryPrevVal = entryPrev.value();
//            auto entryNextVal = entryNext.value();

            newBlockStart = entryPrevVal;
            newBlockPrev = entryPrevVal->getPrev();
            if (coalesceRight) {
                auto entryNextVal = entryNext.value();
                newBlockSize += entryNextVal->getSize() + entryPrevVal->getSize();
                newBlockNext = entryNextVal->getNext();
                if (newBlockNext)
                    *(newBlockNext->prev()) = newBlockStart;
            } else {
                newBlockSize = entryPrevVal->getSize();
                newBlockNext = entryPrevVal->getNext();
            }
        } else {
            auto entryNextVal = entryNext.value();
            newBlockSize += entryNextVal->getSize();
            newBlockPrev = entryNextVal->getPrev();
            if (newBlockPrev)
                *(newBlockPrev->next()) = newBlockStart;

            newBlockNext = entryNextVal->getNext();
            if (newBlockNext)
                *(newBlockNext->prev()) = newBlockStart;
        }

        return HeapBlock::CreateFreeBlockAtMemory(newBlockStart, newBlockSize, newBlockPrev, newBlockNext);
    }

    void FreeList::print() const {
        HeapBlock* curr = m_head;
        fmt::print("--- Free Block State ---\n");
        while (curr != nullptr) {
            curr->print();
            curr = curr->getNext();
        }
    }

    HeapBlock *FreeList::getHead() const {
        return m_head;
    }

    HeapBlock *FreeList::getTail() const {
        return m_tail;
    }

    size_t FreeList::size() const {
        return m_size;
    }

    HeapBlock *FreeList::findNextFree(HeapBlock* block, const void* end) const {
        return nullptr;
    }


}