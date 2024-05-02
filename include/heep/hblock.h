#ifndef HEADER_BLOCK_H
#define HEADER_BLOCK_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


struct HeaderBlock {
    uint32_t size_free_pack;
    struct HeaderBlock* next;
    struct HeaderBlock* prev;
};

typedef struct HeaderBlock HBlock;

#define FREE_HEADER_SIZE (sizeof(HBlock))
#define ALLOC_HEADER_SIZE (sizeof(uint32_t))

size_t block_get_size(const HBlock* block);
bool block_get_free(const HBlock* block);
void* block_get_end_addr(const HBlock* block);
void* block_get_start_addr_from_data(void* ptr);
void block_set_size(HBlock* block, size_t size);
void block_set_free(HBlock* block, HBlock* next, HBlock* prev);
void block_init(HBlock* block, size_t size, bool free, HBlock* next, HBlock* prev);
void block_expand(HBlock* block, size_t increment);
void block_print(const HBlock* block);

#endif