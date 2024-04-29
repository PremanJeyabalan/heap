#include "heep/hblock.h"

size_t get_size(const HeaderBlock* block) {
    return block->size_free_pack & 0xFFFFFFF8;
}

bool get_free(const HeaderBlock* block) {
    return block->size_free_pack & 0x01;
}

void* get_end_addr_block(const HeaderBlock* block) {
    return (void*)((const char*)block + get_size(block));
}

void* get_start_addr_block_from_data(void* ptr) {
    return (void*)((char*)ptr - ALLOC_HEADER_SIZE);
}

void set_size(HeaderBlock* block, size_t size) {
    block->size_free_pack = size | get_free(block);    
}

void set_free(HeaderBlock* block, HeaderBlock* next, HeaderBlock* prev) {
    block->size_free_pack |= 0x01;
    block->next = next;
    block->prev = prev;
}

void make_block(HeaderBlock* block, size_t size, bool free, HeaderBlock* next, HeaderBlock* prev) {
    set_size(block, size);
    if (free) 
        set_free(block, next, prev);
    else
        block->size_free_pack = get_size(block);
}

void expand_block(HeaderBlock* block, size_t increment) {
    size_t newSize = get_size(block) + increment;
    set_size(block, newSize);
}

void print_block(const HeaderBlock* block) {
    void* end = get_end_addr_block(block);
    printf("  [BLOCK %p-%p] %ld\t[%s]\n", 
        (void*)block, (void*)end, get_size(block), get_free(block) ? "FREE" : "USED");
}