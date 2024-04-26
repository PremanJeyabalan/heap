#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define BLOCK_SIZE 16

struct HeaderBlock {
    uint size_free_pack;
    struct HeaderBlock* next;
    struct HeaderBlock* prev;
};

typedef struct HeaderBlock HeaderBlock;
#define FREE_HEADER_SIZE (sizeof(HeaderBlock))
#define ALLOC_HEADER_SIZE (sizeof(uint))

size_t get_size(const HeaderBlock* block) {
    return block->size_free_pack & 0xFFFFFFF8;
}

bool get_free(const HeaderBlock* block) {
    return block->size_free_pack & 0x01;
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

void print_block(const HeaderBlock* block) {
    const HeaderBlock* end = block + get_size(block);
    printf("  [BLOCK %p-%p] %ld\t[%s]\n", 
        (void*)block, (void*)end, get_size(block), get_free(block) ? "FREE" : "USED");
}