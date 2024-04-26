
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include "hblock.h"

#define DWORD_SIZE 8
#define GET_ALIGNED_SIZE(x) ((((x) - 1) | 15) + 1)


typedef struct {
    void* memory;
    size_t size;
    size_t capacity;
    HeaderBlock* freeList;
} Heap;

static Heap heap;

void make_heap(size_t bytes) {
    if ((heap.memory = sbrk(FREE_HEADER_SIZE + bytes)) == (void*) -1) {
        printf("Failed to allocated: %ld bytes to the heap\n", bytes);
        exit(1);
    }

    heap.size = 0;
    heap.capacity = bytes;
    heap.freeList = (HeaderBlock*) heap.memory;
    make_block(heap.freeList, bytes, true, NULL, NULL);
}

void print_heap() {
    printf("--- Current Memory State ---\n");
    printf("[REGION %p]\n", heap.memory); 
    HeaderBlock* curr = (HeaderBlock*) heap.memory;
    HeaderBlock* end = curr + heap.capacity;
    while (curr < end) {
        print_block(curr);
        if (get_size(curr) == 0)
            return;
        curr += get_size(curr);
    }
}

void print_heap_free_blocks() {
    HeaderBlock* curr = heap.freeList;
    while (curr != NULL) {
        print_block(curr);
        curr = curr->next;
    }
}

void* ff_malloc(size_t bytes) {
    HeaderBlock* curr = (HeaderBlock*) heap.memory;
    HeaderBlock* end = curr + heap.capacity;
    size_t alignedSize = GET_ALIGNED_SIZE(bytes + ALLOC_HEADER_SIZE);
    printf("[ALLOC] req size: %ld aligned size: %ld\n", bytes, alignedSize);
    while (curr < end) {
        if (get_free(curr)
            && get_size(curr) >= alignedSize
        ) {
            size_t remFreeSize = get_size(curr) - alignedSize;
            if (remFreeSize > FREE_HEADER_SIZE)
                make_block(curr + alignedSize, remFreeSize, true, curr->next, curr->prev);

            make_block(curr, alignedSize, false, NULL, NULL);
            heap.size += alignedSize;
            return (void*)(curr+ALLOC_HEADER_SIZE);
        }

        curr += get_size(curr);
    }

    printf("Couldn't find any space to allocate %ld bytes aligned to %ld bytes\n", bytes, alignedSize);
    return (void*)-1;
}

void ff_free(void* ptr) {
    printf("Trying to free ptr %p bytes...\n", ptr);

    
}