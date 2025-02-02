//
// Created by prema on 9/6/2024.
//

#include <fmt/format.h>

#include "heep/FreeList.hpp"
#include "heep/Helpers.hpp"

namespace heep {
    FreeList::FreeList(void *memory, size_t size) :
    m_head{HeapBlock::CreateFreeBlockAtMemory(memory, helpers::size_and_free_to_pack(size, true), nullptr, nullptr)}, m_tail{m_head}, m_size{size}
    {
        m_head->init(helpers::size_and_free_to_pack(size, true), nullptr, nullptr);
    }

    HeapBlock *FreeList::pushBack(void *memory, size_t size) {
//        fmt::print("Pushing back for {} with tail {}\n", memory, (void*)m_tail);
        auto* block = HeapBlock::CreateFreeBlockAtMemory(memory, size, m_tail, nullptr);
//        fmt::print("Pushing back for {} with tail {} , now has prev {}\n", memory, (void*)m_tail, (void*)(block->getPrev()));
        if (m_tail != nullptr)
            m_tail->setNext(block);
        else
            m_head = block;

        m_tail = block;
        m_size += size;
        return m_tail;
    }

    HeapBlock *FreeList::pushFront(void *memory, size_t size) {
        auto* block = HeapBlock::CreateFreeBlockAtMemory(memory, size, nullptr, m_head);
        if (m_head != nullptr)
            m_head->setPrev(block);

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
            m_tail->setNext(nullptr);
        else
            m_head = nullptr;
    }

    void FreeList::popFront() {
        if (m_head == nullptr)
            return;

        m_size -= m_head->getSize();
        m_head = m_head->getNext();
        if (m_head != nullptr)
            m_head->setPrev(nullptr);
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

            prev->setNext(next);
            next->setPrev(prev);
        }
    }

    HeapBlock* FreeList::insert(void* memory, size_t size, const void* start, const void* end) {
        if (m_head == nullptr)
            return pushBack(memory, size);

        m_size += size;

        auto* entry = static_cast<HeapBlock*>(memory);
        auto [entryPrev, entryNext] = HeapBlock::GetNeighbours(entry, size, start, end);
        bool coalesceRight = entryNext.has_value() && entryNext.value()->getFree();
        bool coalesceLeft = entryPrev.has_value() && entryPrev.value()->getFree();

        auto* newBlockStart = entry;
        size_t newBlockSize = size;
        HeapBlock* newBlockNext{};
        HeapBlock* newBlockPrev{};

        if (!coalesceLeft) {
            //new block is being inserted into the list
            auto [prev, next] = [&]() -> std::pair<HeapBlock*, HeapBlock*> {
                if (!coalesceRight)
                    return findPrevAndNextFree(newBlockStart);

//                fmt::print("Right block at address: {}\n", (void*)entryNext.value());

                auto* nextVal = entryNext.value();
//                fmt::print("Prev of right block: {}\n", (void*)nextVal->getPrev());
//                fmt::print("Head of free list: {}\n", (void*)m_head);
//                fmt::print("Next of head of free list: {}\n", (void*)m_head->getNext());

                newBlockSize +=  entryNext.value()->getSize();
                return std::make_pair<HeapBlock*, HeapBlock*>(entryNext.value()->getPrev(), entryNext.value()->getNext());
            }();

            newBlockPrev = prev;
            if (newBlockPrev)
                newBlockPrev->setNext(newBlockStart);
            else {
                if (m_head)
                    m_head->setPrev(newBlockStart);

                m_head = newBlockStart;
            }

            newBlockNext = next;
            if (newBlockNext)
                newBlockNext->setPrev(newBlockStart);
            else {
                if (m_tail)
                    m_tail->setNext(newBlockStart);

                m_tail = newBlockStart;
            }

        } else {
            auto entryPrevVal = entryPrev.value();

            newBlockStart = entryPrevVal;
            newBlockPrev = entryPrevVal->getPrev();
            if (coalesceRight) {
                auto entryNextVal = entryNext.value();
                newBlockSize += entryNextVal->getSize() + entryPrevVal->getSize();
                newBlockNext = entryNextVal->getNext();
                if (newBlockNext)
                    newBlockNext->setPrev(newBlockStart);
//                    *(newBlockNext->prev()) = newBlockStart;
                else
                    m_tail = newBlockStart;
            } else {
                newBlockSize += entryPrevVal->getSize();
                newBlockNext = entryPrevVal->getNext();
            }
        }

        return HeapBlock::CreateFreeBlockAtMemory(newBlockStart, newBlockSize, newBlockPrev, newBlockNext);
    }

    std::optional<HeapBlock*> FreeList::resizeBlock(HeapBlock* block, size_t newSize) {
        if (newSize == 0) {
            erase(block);
            return std::nullopt;
        }

        size_t originalSize = block->getSize();
        block = HeapBlock::CreateFreeBlockAtMemory(block, newSize, block->getPrev(), block->getNext());
        m_size -= (originalSize - newSize);
        return block;
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

    std::pair<HeapBlock*, HeapBlock*> FreeList::findPrevAndNextFree(HeapBlock *block) const {
        if (block > m_tail)
            return std::make_pair(m_tail, nullptr);
        else if (block < m_head)
            return std::make_pair(nullptr, m_head);

        HeapBlock* prev = [&]() -> HeapBlock* {
            auto* prevBlock = block->getPrevStartAddr();
            while (prevBlock >= m_head) {
                if (prevBlock->getFree())
                    return prevBlock;

                prevBlock = prevBlock->getPrevStartAddr();
            }

            return nullptr;
        }();

        return std::make_pair(prev, prev->getNext());


//        if (block > m_tail)
//            return std::make_pair(m_tail, nullptr);
//
//        auto* currPrev = m_head;
//        while (block > currPrev) {
//            currPrev = currPrev->getNext();
//        }

//        return std::make_pair(currPrev->getPrev(), currPrev);
    }


}