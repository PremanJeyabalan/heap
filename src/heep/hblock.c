#include "heep/hblock.h"


void block_init(HBlock* block, size_t size, bool free, HBlock* next, HBlock* prev) {
    SET_BLOCK_SIZE_FREE_PACK(block, size, free);
    SET_BLOCK_FOOTER(block, block->size_free_pack);
    block->next = next;
    block->prev = prev;
}

void block_expand(HBlock* block, size_t increment) {
    size_t newSize = GET_BLOCK_SIZE(block) + increment;
    SET_BLOCK_SIZE(block, newSize);
    SET_BLOCK_FOOTER(block, block->size_free_pack);
}

void block_print(const HBlock* block) {
    void* end = GET_BLOCK_END_ADDRESS(block);
    printf("  [BLOCK %p-%p] %u\t[%s]\n",
           (void*)block, (void*)end, GET_BLOCK_SIZE(block), GET_BLOCK_FREE(block) ? "FREE" : "USED");
}