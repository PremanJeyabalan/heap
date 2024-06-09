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