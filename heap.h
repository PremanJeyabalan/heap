
#ifndef HEAP_H
#define HEAP_H
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "hblock.h"
#include "flist.h"
#include "mem.h"

#define GET_ALIGED_SIZE_16(x) ((((x) - 1) | 15) + 1)
#define GET_ALIGED_SIZE_PAGE(x) ((((x) - 1) | (PAGE_SIZE-1)) + 1)


typedef struct {
    void* memory;
    size_t size;
    size_t capacity;
    FreeList freeList;
} Heap;


static Heap heap;

void* reserve_heap(size_t bytes) {
    void* top;
    if ((top = mem_sbrk(bytes)) == (void*)-1) {
        printf("[ERROR] Failed to allocate: %ld bytes to the heap\n", bytes);
        exit(1);
    }

    heap.capacity += bytes;
    return top;
}

void make_heap(size_t bytes) {
    mem_init();
    size_t alignedCapacity = GET_ALIGED_SIZE_16(FREE_HEADER_SIZE + bytes);
    heap.memory = reserve_heap(alignedCapacity);
    heap.size = 0;
    list_init(&heap.freeList, heap.memory, heap.capacity);
}

void print_heap() {
    printf("--- Current Memory State ---\n");
    // printf("[STATS]");
    printf("[REGION %p]\n", heap.memory); 
    void* curr = heap.memory;
    void* end = (char*)curr + heap.capacity;
    while (curr < end) {
        print_block(curr);
        if (get_size(curr) == 0)
            break;
        curr = get_end_addr_block((HeaderBlock*)curr);
    }
    list_print(&heap.freeList);
    printf("-----------------------------\n");
}

HeaderBlock* expand_heap(size_t bytes) {
    size_t incrCap = GET_ALIGED_SIZE_16(bytes);
    void* next = reserve_heap(incrCap); 
    HeaderBlock* res;
    if (heap.freeList.tail == NULL || get_end_addr_block(heap.freeList.tail) < next) {
        // printf("[INFO] Trying append at address: %p\n", next);
        res = list_append(&heap.freeList, next, incrCap);
    } else {
        expand_block(heap.freeList.tail, incrCap);
        res = heap.freeList.tail;
    }

    // print_heap();
    return res;
}


void print_heap_free_blocks() {
    HeaderBlock* curr = heap.freeList.head;
    while (curr != NULL) {
        print_block(curr);
        curr = curr->next;
    }
}

HeaderBlock* ff_find_fit(size_t size) {
    HeaderBlock* curr = heap.freeList.head;
    while (curr != NULL) {
        if (get_size(curr) >= size)
            return curr;

        curr = curr->next;
    }

    return NULL;
}

void* ff_malloc(size_t bytes) {
    size_t alignedSize = GET_ALIGED_SIZE_16(bytes + ALLOC_HEADER_SIZE);
    printf("[ALLOC] req size: %ld aligned size: %ld\n", bytes, alignedSize);

    HeaderBlock* victim = ff_find_fit(alignedSize);

    if (victim == NULL) {
        // printf("[INFO] Ran out of space.. expanding heap by %lu\n", alignedSize*2);
        victim = expand_heap(alignedSize*2);
    }

    size_t startOffset = get_size(victim) - alignedSize;
    HeaderBlock* curr = (HeaderBlock*)((char*) victim + startOffset);
    if (startOffset == 0) 
       list_remove(&heap.freeList, victim); 
    else 
        set_size(victim, startOffset);

    make_block(curr, alignedSize, false, NULL, NULL);
    heap.size += alignedSize;
    return (void*)(curr+ALLOC_HEADER_SIZE);
}

void ff_free(void* ptr) {
    printf("Trying to free ptr %p bytes...\n", ptr);

    
}

#endif