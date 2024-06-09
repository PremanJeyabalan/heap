//
// Created by prema on 9/6/2024.
//

#include <fmt/format.h>

#include "heep/HeapBlock.hpp"
#include "heep/Helpers.hpp"

namespace heep {
    HeapBlock::HeapBlock(uint32_t size, bool free, HeapBlock *next, HeapBlock *prev) :
        m_sizeFreePack{helpers::size_and_free_to_pack(size, free)}, m_next{next}, m_prev{prev}
    {
        setFooter();
    }

    void HeapBlock::init(uint32_t pack, HeapBlock *next, HeapBlock *prev) {
        m_sizeFreePack = pack;
        m_next = next;
        m_prev = prev;
        setFooter();
    }

    void HeapBlock::expand(size_t increment) {
        setSize(getSize() + increment);
        getFooterAddr()->m_sizeFreePack = m_sizeFreePack;
    }

    void HeapBlock::assign(HeapBlock *other) {
        m_sizeFreePack = other->m_sizeFreePack;
        m_next = other->m_next;
        m_prev = other->m_prev;
    }

    void HeapBlock::print() const {
        void* end = getEndAddr();

        fmt::print("  [BLOCK {}-{}] {}\t[{}]\n",
                   (void*)this, end, getSize(), getFree() ? "FREE" : "USED");
    }

    uint32_t HeapBlock::getSize() const {
        return helpers::get_size_from_pack(m_sizeFreePack);
    }

    bool HeapBlock::getFree() const {
        return helpers::get_free_from_pack(m_sizeFreePack);
    }

    HeapBlock *HeapBlock::getNext() const {
        return m_next;
    }

    HeapBlock *HeapBlock::getPrev() const {
        return m_prev;
    }

    void *HeapBlock::getEndAddr() const {
        return helpers::get_block_end_address_from_start((void *) this, getSize());
    }

    HeapBlock *HeapBlock::getFooterAddr() const {
        return static_cast<HeapBlock*>(helpers::get_block_footer_address_from_start((void *) this, getSize()));
    }

    void HeapBlock::setSize(uint32_t size) {
        m_sizeFreePack = size | (uint32_t) getFree();
    }

    void HeapBlock::setFree(bool free) {
        m_sizeFreePack = getSize() | (uint32_t) free;
    }

    void HeapBlock::setPack(uint32_t size, bool pack) {
        m_sizeFreePack = helpers::size_and_free_to_pack(size, pack);
    }

    void HeapBlock::setFooter() {
        getFooterAddr()->assign(this);
    }


}