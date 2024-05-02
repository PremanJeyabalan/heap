#include "heep/hblock.h"

size_t block_get_size(const HBlock* block) {
    return block->size_free_pack & 0xFFFFFFF8;
}

bool block_get_free(const HBlock* block) {
    return block->size_free_pack & 0x01;
}

void* block_get_end_addr(const HBlock* block) {
    return (void*)((const char*)block + block_get_size(block));
}

void* block_get_start_addr_from_data(void* ptr) {
    return (void*)((char*)ptr - ALLOC_HEADER_SIZE);
}

void block_set_size(HBlock* block, size_t size) {
    block->size_free_pack = size | block_get_free(block);
}

void block_set_free(HBlock* block, HBlock* next, HBlock* prev) {
    block->size_free_pack |= 0x01;
    block->next = next;
    block->prev = prev;
}

void block_init(HBlock* block, size_t size, bool free, HBlock* next, HBlock* prev) {
    block_set_size(block, size);
    if (free)
        block_set_free(block, next, prev);
    else
        block->size_free_pack = block_get_size(block);
}

void block_expand(HBlock* block, size_t increment) {
    size_t newSize = block_get_size(block) + increment;
    block_set_size(block, newSize);
}

void block_print(const HBlock* block) {
    void* end = block_get_end_addr(block);
    printf("  [BLOCK %p-%p] %ld\t[%s]\n",
           (void*)block, (void*)end, block_get_size(block), block_get_free(block) ? "FREE" : "USED");
}