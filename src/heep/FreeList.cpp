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



}