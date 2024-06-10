//
// Created by prema on 9/6/2024.
//

#ifndef HEEP_HELPERS_HPP
#define HEEP_HELPERS_HPP

#include <cstdlib>

#include "HeapBlock.hpp"

namespace heep::helpers {
    static constexpr size_t get_aligned_16_bytes(size_t bytes) {
        return (((bytes - 1) | 15) + 1);
    }

    static constexpr size_t free_header_size = get_aligned_16_bytes(sizeof(HeapBlock));
    static constexpr size_t alloc_header_size = get_aligned_16_bytes(sizeof(uint32_t));
    static constexpr size_t free_footer_size = get_aligned_16_bytes(sizeof(HeapBlock));
//    static constexpr size_t alloc_footer_size = get_aligned_16_bytes(sizeof(uint32_t));

    static constexpr uint32_t pack_mask = 0xFFFFFFF0;

    static constexpr size_t get_aligned_block(size_t bytes) {
        return get_aligned_16_bytes(alloc_header_size + get_aligned_16_bytes(bytes) + free_footer_size);
    }

    static constexpr uint32_t get_size_from_pack(uint32_t pack) {
        return pack & pack_mask;
    }
    static constexpr bool get_free_from_pack(uint32_t pack) {
        return static_cast<bool>(pack & 0x01);
    }
    static constexpr uint32_t size_and_free_to_pack(uint32_t size, bool free) {
        return size | static_cast<uint32_t>(free);
    }

    static constexpr void* get_block_end_address_from_start(void* ptr, uint32_t size) {
        return static_cast<void*>(
                static_cast<char*>(ptr) + size);
    }
    static constexpr void* get_block_footer_address_from_start(void* ptr, uint32_t size) {
        return static_cast<void*>(
                static_cast<char*>(get_block_end_address_from_start(ptr, size)) - free_footer_size);
    }
    static constexpr void* get_block_start_address_from_data(void* ptr) {
        return static_cast<void*>(static_cast<char*>(ptr) - alloc_header_size);
    }
    static constexpr void* get_block_start_address_from_footer(void* ptr, uint32_t size) {
        return static_cast<void*>(static_cast<char*>(ptr) + free_footer_size - size);
    }
    static constexpr void* get_block_prev_footer_address(void* ptr) {
        return static_cast<void*>(static_cast<char*>(ptr) - free_footer_size);
    }
    static constexpr void* get_block_prev_start_address(void* ptr) {
        void* prevFooter = get_block_prev_footer_address(ptr);
        return get_block_start_address_from_footer(prevFooter, get_size_from_pack(*static_cast<uint32_t*>(prevFooter)));
    }
}

#endif //HEEP_HELPERS_HPP
