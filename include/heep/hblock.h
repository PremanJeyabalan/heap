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

typedef struct HeaderBlock HeaderBlock;

#define BLOCK_SIZE 16
#define FREE_HEADER_SIZE (sizeof(HeaderBlock))
#define ALLOC_HEADER_SIZE (sizeof(uint32_t))

size_t get_size(const HeaderBlock* block);
bool get_free(const HeaderBlock* block);
void* get_end_addr_block(const HeaderBlock* block);
void* get_start_addr_block_from_data(void* ptr);
void set_size(HeaderBlock* block, size_t size);
void set_free(HeaderBlock* block, HeaderBlock* next, HeaderBlock* prev);
void make_block(HeaderBlock* block, size_t size, bool free, HeaderBlock* next, HeaderBlock* prev);
void expand_block(HeaderBlock* block, size_t increment);
void print_block(const HeaderBlock* block);

#endif