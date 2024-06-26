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

    HeapBlock *HeapBlock::CreateFreeBlockAtMemory(void *memory, size_t size, HeapBlock *prev, HeapBlock *next) {
        auto* res = static_cast<HeapBlock*>(memory);
        res->init(helpers::size_and_free_to_pack(size, true), next, prev);
        return res;
    }

    HeapBlock *HeapBlock::CreateAllocBlockAtMemory(void *memory, size_t size) {
        auto* res = static_cast<HeapBlock*>(memory);
        res->init(helpers::size_and_free_to_pack(size, false), nullptr, nullptr);
        return res;
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

    HeapBlock** HeapBlock::next() {
        return &m_next;
    }

    HeapBlock** HeapBlock::prev() {
        return &m_prev;
    }

    void *HeapBlock::getEndAddr() const {
        return helpers::get_block_end_address_from_start((void *) this, getSize());
    }

    void *HeapBlock::getDataAddr() const {
        return helpers::get_block_data_address_from_start((void*)this);
    }

    HeapBlock *HeapBlock::getFooterAddr() const {
        return static_cast<HeapBlock*>(helpers::get_block_footer_address_from_start((void *) this, getSize()));
    }

    HeapBlock *HeapBlock::getPrevStartAddr() const {
        return static_cast<HeapBlock*>(helpers::get_block_prev_start_address((void*)this));
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

    std::pair<
            std::optional<HeapBlock *>, std::optional<HeapBlock *>
    > HeapBlock::GetNeighbours(HeapBlock* block, size_t size, const void* startBoundary, const void* endBoundary) {
        std::optional<HeapBlock*> prevOpt = [](HeapBlock* b, const void* startBdry) -> std::optional<HeapBlock*> {
            if (b == startBdry)
                return std::nullopt;

            return (HeapBlock*) helpers::get_block_prev_start_address(b);
        }(block, startBoundary);

        std::optional<HeapBlock*> nextOpt = [](HeapBlock* b, size_t size, const void* endBdry) -> std::optional<HeapBlock*> {
            void* endAddr = helpers::get_block_end_address_from_start(b, size);
            if (endAddr == endBdry)
                return std::nullopt;

            return (HeapBlock*) endAddr;
        }(block, size, endBoundary);

        return std::make_pair(prevOpt, nextOpt);
    }
}