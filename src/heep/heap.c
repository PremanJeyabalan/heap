#include "heep/heap.h"
#include "vmem/vmem.h"

#define GET_ALIGNED_SIZE_16(x) ((((x) - 1) | 15) + 1)
#define GET_ALIGNED_SIZE_PAGE(x) ((((x) - 1) | (PAGE_SIZE-1)) + 1)

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

static HeaderBlock* expand_heap(size_t bytes) {
    size_t incrCap = GET_ALIGNED_SIZE_16(bytes);
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

static HeaderBlock* ff_find_fit(size_t size) {
    HeaderBlock* curr = heap.freeList.head;
    while (curr != NULL) {
        if (get_size(curr) >= size)
            return curr;

        curr = curr->next;
    }

    return NULL;
}

void* ff_malloc(size_t bytes) {
    size_t alignedSize = GET_ALIGNED_SIZE_16(bytes + FREE_HEADER_SIZE);
    // printf("[ALLOC] req size: %ld aligned size: %ld\n", bytes, alignedSize);

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
    return (void*)((char*)(curr)+ALLOC_HEADER_SIZE);
}

void ff_free(void* ptr) {
    HeaderBlock* block = get_start_addr_block_from_data(ptr);
    // printf("[FREE] Free block: %p\n", (void*)block);
    size_t size = get_size(block);
    make_block(block, size, true, NULL, NULL);

    list_insert_and_coalesce(&heap.freeList, block);
    heap.size -= size;
}

void make_heap(size_t bytes) {
    mem_init();
    size_t alignedCapacity = GET_ALIGNED_SIZE_16(FREE_HEADER_SIZE + bytes);
    heap.memory = reserve_heap(alignedCapacity);
    heap.size = 0;
    list_init(&heap.freeList, heap.memory, heap.capacity);
}

void collect_heap() {
    mem_cleanup();
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