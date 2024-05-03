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

#define GET_ALIGNED_SIZE_16(x) ((((x) - 1) | 15) + 1)

#define FREE_HEADER_SIZE (GET_ALIGNED_SIZE_16(sizeof(HBlock)))
#define ALLOC_HEADER_SIZE (GET_ALIGNED_SIZE_16(sizeof(uint32_t)))
#define FOOTER_SIZE (sizeof(uint32_t))

#define GET_ALIGNED_BLOCK(x) (GET_ALIGNED_SIZE_16(ALLOC_HEADER_SIZE + GET_ALIGNED_SIZE_16((x)) + FOOTER_SIZE))

#define GET_SIZE_FROM_PACK(pack) ((uint32_t)((pack) & 0xFFFFFFF8))
#define GET_FREE_FROM_PACK(pack) ((bool)((pack) & 0x01))

#define GET_BLOCK_SIZE(ptr) ((uint32_t)GET_SIZE_FROM_PACK(((HBlock*)(ptr))->size_free_pack))
#define GET_BLOCK_FREE(ptr) (GET_FREE_FROM_PACK(((HBlock*)(ptr))->size_free_pack))

#define GET_BLOCK_END_ADDRESS(ptr) ((void*)((const char*)(ptr) + GET_BLOCK_SIZE((HBlock*)(ptr))))
#define GET_BLOCK_FOOTER_ADDRESS(ptr) ((void*)((char*)(GET_BLOCK_END_ADDRESS((ptr)) - FOOTER_SIZE)))
#define GET_BLOCK_START_ADDRESS_FROM_DATA(ptr) ((void*)((char*)(ptr) - ALLOC_HEADER_SIZE))
#define GET_BLOCK_START_ADDRESS_FROM_FOOTER(ptr) ((void*)((((char*)(ptr)) + FOOTER_SIZE) - GET_SIZE_FROM_PACK(((HBlock*)(ptr))->size_free_pack)))
#define GET_PREVIOUS_BLOCK_FOOTER_ADDRESS(ptr) ((void*)(((char*)(ptr)) - FOOTER_SIZE))
#define GET_PREVIOUS_BLOCK_ADDRESS(ptr)(GET_BLOCK_START_ADDRESS_FROM_FOOTER(GET_PREVIOUS_BLOCK_FOOTER_ADDRESS((ptr))))

#define SET_BLOCK_SIZE(ptr, size) (((HBlock*)(ptr))->size_free_pack = (uint32_t)((size) | GET_BLOCK_FREE((ptr)))
#define SET_BLOCK_FREE(ptr, free) (((HBlock*)(ptr))->size_free_pack |= 0x01)
#define SET_BLOCK_SIZE_FREE_PACK(block, size, free) (((HBlock*)(block))->size_free_pack = (uint32_t)(size) | (uint32_t)(free))
#define SET_BLOCK_FOOTER(ptr, pack) (*((uint32_t*)(GET_BLOCK_FOOTER_ADDRESS((void*)(ptr)))) = (pack))

void block_init(HBlock* block, size_t size, bool free, HBlock* next, HBlock* prev);
void block_expand(HBlock* block, size_t increment);
void block_print(const HBlock* block);

#endif