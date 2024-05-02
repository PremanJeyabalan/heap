//
// Created by prema on 1/5/2024.
//
#include <string.h>

#include "../src/heep/heap.c"

int test_heap_init() {
    heap_init(4096);
    int res=0;
    if (heap.size > 0 ||
        heap.memory == NULL ||
        heap.freeList.size != 4096+32 ||
        heap.freeList.head == NULL ||
        heap.freeList.tail != heap.freeList.head ||
            block_get_size(heap.freeList.head) != heap.freeList.size ||
        heap.freeList.head->next != NULL ||
        heap.freeList.head->prev != NULL)
    {
        res = 1;
    }

    heap_collect();
    return res;
}

int test_heap_malloc_from_empty() {
    heap_init(64);
    void* ptr = heap_malloc(16);
    FreeList* fList = &heap.freeList;
    HBlock* block = block_get_end_addr(fList->head);
    int res=0;
    if (heap.size != 48 ||
        heap.capacity != 96 ||
        (void*)fList->head != heap.memory ||
        fList->size != 48 ||
            block_get_size(fList->head) != 48 || !block_get_free(fList->head) ||
            block_get_size(block) != 48 || block_get_free(block) ||
        ptr != (void*)((char*)block + sizeof(uint32_t)))
        res = 1;

    heap_collect();
    return res;
}

int test_heap_malloc_to_full() {
    int res=0;

    heap_init(64);
    void* ptr = heap_malloc(16);
    ptr = heap_malloc(16);

    FreeList* fList = &heap.freeList;
    HBlock* block = heap.memory;

    if (heap.size != 96 ||
        heap.capacity != 96 ||
        (void*)fList->head != NULL || (void*)fList->tail != NULL ||
        fList->size != 0 ||
            block_get_size(block) != 48 || block_get_free(block) ||
        ptr != (void*)((char*)block + sizeof(uint32_t)))
        res = 1;

    heap_collect();
    return res;
}

int test_heap_malloc_expand_coalesce() {
    int res=0;

    heap_init(64);
    HBlock* incrPtr = expand_heap(16);
    FreeList* fList = &heap.freeList;

    if (heap.capacity != 128 ||
        heap.size != 0 ||
        (void*)incrPtr != heap.memory ||
        fList->size != 128 ||
        fList->head != incrPtr || fList->tail != incrPtr ||
        fList->head->next != NULL || fList->head->prev != NULL ||
            block_get_size(fList->head) != 128 || !block_get_free(fList->head))
        res = 1;

    heap_collect();
    return res;
}

int test_heap_malloc_expand_no_coalesce() {
    int res=0;

    heap_init(64);
    FreeList* fList = &heap.freeList;

    void* ptr = heap_malloc(16);
    HBlock* incrPtr = expand_heap(16);

    if (heap.capacity != 128 ||
        heap.size != 48 ||
        fList->size != 80 ||
        (void*)fList->head != heap.memory || block_get_size(fList->head) != 48 || !block_get_free(fList->head) || fList->head->next != fList->tail ||
        incrPtr != fList->tail || fList->tail->prev != fList->head || (void*)fList->tail != (void*)((char*)heap.memory + 96) ||
            block_get_size(fList->tail) != 32 || !block_get_free(fList->tail))
        res = 1;

    heap_collect();
    return res;
}

int test_ff_find_fit_order_first() {
    int res=0;

    heap_init(64);
    FreeList* fList = &heap.freeList;

    void* ptr = heap_malloc(16);
    HBlock* incrPtr = expand_heap(16);

    size_t alignedSize = GET_ALIGNED_SIZE_16(16 + FREE_HEADER_SIZE);
    HBlock* victim = ff_find_fit(alignedSize);
    if (block_get_size(victim) < 16 ||
        !block_get_free(victim) ||
        (void*)victim != heap.memory)
        res = 1;

    heap_collect();
    return res;
}

int test_ff_find_fit_order_last() {
    int res=0;

    heap_init(64);
    FreeList* fList = &heap.freeList;

    void* ptr = heap_malloc(16);
    HBlock* incrPtr = expand_heap(64);

    HBlock* victim = ff_find_fit(64);
    if (block_get_size(victim) < 64 ||
        !block_get_free(victim) ||
        (void*)victim != (void*)((char*)heap.memory + 96))
        res = 1;

    heap_collect();
    return res;
}

int test_ff_find_fit_order_none() {
    int res=0;

    heap_init(64);
    FreeList* fList = &heap.freeList;

    void* ptr = heap_malloc(16);
    HBlock* incrPtr = expand_heap(16);

    HBlock* victim = ff_find_fit(64);
    if (victim != NULL)
        res = 1;

    heap_collect();
    return res;
}

int test_heap_free_no_coalesce() {
    int res=0;

    heap_init(160);
    FreeList* fList = &heap.freeList;

    void* ptr = heap_malloc(16);
    HBlock* incrPtr = expand_heap(64);
    void* ptr2 = heap_malloc(16);
    HBlock* block = (HBlock*)((char*)ptr2 - 4);
    void* ptr3 = heap_malloc(16);

    memset(ptr3, 'A', 16);

    heap_free(ptr2);

    if (heap.capacity != 256 ||
        heap.size != 96 ||
        fList->size != 160 ||
        (void*)fList->head != heap.memory || block_get_size(fList->head) != 48 || !block_get_free(fList->head) || fList->head->next == fList->tail ||
        (void*)fList->head->next != (void*)((char*) block_get_end_addr(fList->head) + 48) ||
            block_get_size(fList->head->next) != 48 || !block_get_free(fList->head->next) ||
        (void*)fList->tail != (void*)((char*) block_get_end_addr(fList->head->next) + 48))
        res = 1;

    heap_collect();
    return res;
}

int test_heap_free_left_coalesce() {
    int res=0;

    heap_init(160);
    FreeList* fList = &heap.freeList;

    void* ptr = heap_malloc(16);
    void* ptr2 = heap_malloc(16);
    HBlock* block = (HBlock*)((char*)ptr2 - 4);

    heap_free(ptr2);

    if (heap.capacity != 192 ||
        heap.size != 48 ||
        fList->size != 144 ||
        (void*)fList->head != heap.memory || block_get_size(fList->head) != 144 || !block_get_free(fList->head) || fList->head->next != NULL)
        res = 1;

    heap_collect();
    return res;
}

int test_heap_free_right_coalesce() {
    int res=0;

    heap_init(160);
    FreeList* fList = &heap.freeList;

    void* ptr = heap_malloc(16);
    void* ptr2 = heap_malloc(16);
    HBlock* incrPtr = expand_heap(64);
    HBlock* block = (HBlock*)((char*)ptr - 4);

    heap_free(ptr);

    if (heap.capacity != 256 ||
        heap.size != 48 ||
        fList->size != 208 ||
        (void*)fList->head != heap.memory || block_get_size(fList->head) != 96 || !block_get_free(fList->head) || fList->head->next != fList->tail ||
        fList->tail->prev != fList->head || fList->tail != block ||
            block_get_size(block) != 112 || !block_get_free(block))
        res = 1;

    heap_collect();
    return res;

}

int test_heap_free_all_coalesce() {
    int res=0;

    heap_init(160);
    FreeList* fList = &heap.freeList;

    void* ptr = heap_malloc(16);
    HBlock* incrPtr = expand_heap(64);

    heap_free(ptr);

    if (heap.capacity != 256 ||
        heap.size != 0 ||
        fList->size != 256 ||
        (void*)fList->head != heap.memory || block_get_size(fList->head) != 256 || !block_get_free(fList->head) || fList->head->next != NULL || fList->head->prev != NULL ||
        fList->tail != fList->head)
        res = 1;

    heap_collect();
    return res;
}

int main(void) {
    if (test_heap_init())
        return 1;

    if (test_heap_malloc_from_empty())
        return 1;

    if (test_heap_malloc_to_full())
        return 1;

    if (test_heap_malloc_expand_coalesce())
        return 1;

    if (test_heap_malloc_expand_no_coalesce())
        return 1;

    if (test_ff_find_fit_order_first())
        return 1;

    if (test_ff_find_fit_order_last())
        return 1;

    if (test_ff_find_fit_order_none())
        return 1;

    if (test_heap_free_no_coalesce())
        return 1;

    if (test_heap_free_left_coalesce())
        return 1;

    if (test_heap_free_right_coalesce())
        return 1;

    if (test_heap_free_all_coalesce())
        return 1;

    return 0;
}