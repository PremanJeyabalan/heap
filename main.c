#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

struct Block {
    uint32_t size_free_pack;
    struct Block* next;
    struct Block* prev;
};
typedef struct Block Block;

struct BlockList {
    Block* begin;
    Block* end;
    size_t size;
};
typedef struct BlockList BlockList;

BlockList freeList;
BlockList allocList;

void* ff_malloc(size_t bytes) {
    printf("Trying to alloc %ld bytes...\n", bytes);
    if (freeList.size == 0) {
        freeList.begin = (Block*) sbrk(bytes + sizeof(Block));
        freeList.begin->size_free_pack = ((bytes*bytes) << 3);
        freeList.begin->size_free_pack &= 0x01;
        freeList.begin->next = NULL;
        freeList.begin->prev = NULL;
    }

    Block* curr = freeList.begin;
    while (curr != NULL) {
        if (curr->size_free_pack&0x01 && bytes + sizeof(Block) <= (curr->size_free_pack >> 3)) {
            // curr->size_free_pack = 

        }
    }

    return (void*) NULL;
}

void ff_free(void* ptr) {
    printf("Trying to free ptr %p bytes...\n", ptr);

    
}

#define ALLOCS 10

void* ptrs[ALLOCS] = {0};

int main() {

    for (int i = 0; i < ALLOCS; i++) {
        ptrs[i] = ff_malloc(i);
    }

    for (int i = 0; i < ALLOCS; i++) {
        ff_free(ptrs[i]);
    }

    return 0;
}