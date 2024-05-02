#include <stdio.h>

#include "heep/heap.h"
#include "vmem/vmem.h"

#define GET_ALIGNED_SIZE_16(x) ((((x) - 1) | 15) + 1)
#define GET_ALIGNED_SIZE_PAGE(x) ((((x) - 1) | (PAGE_SIZE-1)) + 1)
#define GET_ALIGNED_BLOCK(x) (GET_ALIGNED_SIZE_16((x)+FREE_HEADER_SIZE))

#ifdef FIRST_FIT_FIND
#define FINDER(x) ff_find_fit(x)
#endif
#ifdef BEST_FIT_FIND
#define FINDER(x) bf_find_fit(x)
#endif

static Heap heap;

static void* reserve_heap(size_t bytes) {
    void* top;
    if ((top = mem_sbrk(bytes)) == (void*)-1) {
        printf("[ERROR] Failed to allocate: %ld bytes to the heap\n", bytes);
        exit(1);
    }

    heap.capacity += bytes;
    return top;
}

static HBlock* expand_heap(size_t bytes) {
    size_t incrCap = bytes > 16 ? GET_ALIGNED_SIZE_16(bytes) : 32;
    void* next = reserve_heap(incrCap); 
    HBlock* res;
    if (heap.freeList.tail == NULL || block_get_end_addr(heap.freeList.tail) < next) {
        // printf("[INFO] Trying append at address: %p\n", next);
        res = list_append(&heap.freeList, next, incrCap);
    } else {
        block_expand(heap.freeList.tail, incrCap);
        heap.freeList.size += incrCap;
        res = heap.freeList.tail;
    }

    // heap_print();
    return res;
}

static HBlock* ff_find_fit(size_t size) {
    HBlock* curr = heap.freeList.head;
    while (curr != NULL) {
        size_t currSize = block_get_size(curr);
        if (currSize >= size)
            return curr;

        curr = curr->next;
    }

    return NULL;
}

static HBlock* bf_find_fit(size_t size) {
    HBlock* curr = heap.freeList.head;
    HBlock* res = NULL;
    while (curr != NULL) {
        size_t currSize = block_get_size(curr);
        if (
                ((currSize == size) || (currSize > size && currSize - size >= 32)) &&
                (res == NULL || currSize < block_get_size(res))) {
            res = curr;
        }

        curr = curr->next;
    }

    return res;
}

void* heap_malloc(size_t bytes) {
    size_t alignedSize = GET_ALIGNED_BLOCK(bytes);
    // printf("[ALLOC] req size: %ld aligned size: %ld\n", bytes, alignedSize);

    HBlock* victim = FINDER(alignedSize);

    if (victim == NULL) {
        // printf("[INFO] Ran out of space.. expanding heap by %lu\n", alignedSize*2);
        victim = expand_heap(alignedSize*2);
    }

    size_t startOffset = block_get_size(victim) - alignedSize;
    HBlock* curr = (HBlock*)((char*) victim + startOffset);
    if (startOffset == 0) 
       list_remove(&heap.freeList, victim); 
    else {
        block_set_size(victim, startOffset);
        heap.freeList.size -= alignedSize;
    }

    block_init(curr, alignedSize, false, NULL, NULL);
    heap.size += alignedSize;
    return (void*)((char*)(curr)+ALLOC_HEADER_SIZE);
}

void heap_free(void* ptr) {
    HBlock* block = block_get_start_addr_from_data(ptr);
    // printf("[FREE] Free block: %p\n", (void*)block);
    size_t size = block_get_size(block);
    block_init(block, size, true, NULL, NULL);

    list_insert_and_coalesce(&heap.freeList, block);
    heap.size -= size;
}

void heap_init(size_t bytes) {
    mem_init();
    size_t alignedCapacity = GET_ALIGNED_BLOCK(bytes);
    heap.memory = reserve_heap(alignedCapacity);
    heap.size = 0;
    list_init(&heap.freeList, heap.memory, heap.capacity);
#ifdef FIRST_FIT_FIND
    printf("Using First Fit Finder for this heap!\n");
#endif
#ifdef BEST_FIT_FIND
    printf("Using Best Fit Finder for this heap!\n");
#endif
}

void heap_collect() {
    mem_cleanup();
    heap.memory = NULL;
    heap.size = 0;
    heap.capacity = 0;
    heap.freeList.size = 0;
    heap.freeList.head = NULL;
    heap.freeList.tail =  NULL;
}

void heap_print() {
    printf("--- Current Memory State ---\n");
    // printf("[STATS]");
    printf("[REGION %p]\n", heap.memory); 
    void* curr = heap.memory;
    void* end = (char*)curr + heap.capacity;
    while (curr < end) {
        block_print(curr);
        if (block_get_size(curr) == 0)
            break;
        curr = block_get_end_addr((HBlock *) curr);
    }
    list_print(&heap.freeList);
    printf("-----------------------------\n");
}