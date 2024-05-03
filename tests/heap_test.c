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
            GET_BLOCK_SIZE(heap.freeList.head) != heap.freeList.size ||
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
    HBlock* block = GET_BLOCK_END_ADDRESS(fList->head);
    int res=0;
    if (heap.size != 48 ||
        heap.capacity != 96 ||
        (void*)fList->head != heap.memory ||
        fList->size != 48 ||
            GET_BLOCK_SIZE(fList->head) != 48 || !GET_BLOCK_FREE(fList->head) ||
            GET_BLOCK_SIZE(block) != 48 || GET_BLOCK_FREE(block) ||
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
            GET_BLOCK_SIZE(block) != 48 || GET_BLOCK_FREE(block) ||
        ptr != (void*)((char*)block + sizeof(uint32_t)))
        res = 1;

    heap_collect();
    return res;
}

int test_heap_malloc_expand_coalesce() {
    int res=0;

    heap_init(64);
    size_t oldCapacity = heap_get_capacity();
    size_t newExpectedCapacity = oldCapacity*2;
    HBlock* incrPtr = expand_heap(16);
    FreeList* fList = &heap.freeList;

    if (heap.capacity != newExpectedCapacity ||
        heap.size != 0 ||
        (void*)incrPtr != heap.memory ||
        fList->size != newExpectedCapacity ||
        fList->head != incrPtr || fList->tail != incrPtr ||
        fList->head->next != NULL || fList->head->prev != NULL ||
            GET_BLOCK_SIZE(fList->head) != newExpectedCapacity || !GET_BLOCK_FREE(fList->head))
        res = 1;

    heap_collect();
    return res;
}

int test_heap_malloc_expand_no_coalesce() {
    int res=0;

    heap_init(64);
    size_t oldCapacity = heap_get_capacity();
    FreeList* fList = &heap.freeList;

    void* ptr = heap_malloc(16);
    HBlock* incrPtr = expand_heap(16);
    size_t newExpectedCapacity = oldCapacity*2;

    if (heap.capacity != newExpectedCapacity ||
        heap.size != 48 ||
        fList->size != newExpectedCapacity-48 ||
        (void*)fList->head != heap.memory || GET_BLOCK_SIZE(fList->head) != 48 || !GET_BLOCK_FREE(fList->head) || fList->head->next != fList->tail ||
        incrPtr != fList->tail || fList->tail->prev != fList->head || (void*)fList->tail != (void*)((char*)heap.memory + 96) ||
            GET_BLOCK_SIZE(fList->tail) != newExpectedCapacity-48-48 || !GET_BLOCK_FREE(fList->tail))
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
    if (GET_BLOCK_SIZE(victim) < 16 ||
        !GET_BLOCK_FREE(victim) ||
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
    if (GET_BLOCK_SIZE(victim) < 64 ||
        !GET_BLOCK_FREE(victim) ||
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

    HBlock* victim = ff_find_fit(97);
    if (victim != NULL)
        res = 1;

    heap_collect();
    return res;
}

int test_heap_free_no_coalesce() {
    int res=0;

    heap_init(160);
    size_t oldCapacity = 192;
    FreeList* fList = &heap.freeList;

    void* ptr = heap_malloc(16);
    HBlock* incrPtr = expand_heap(64);
    size_t newExpectedCapacity = oldCapacity*2;
    void* ptr2 = heap_malloc(16);
    HBlock* block = (HBlock*)((char*)ptr2 - 4);
    void* ptr3 = heap_malloc(16);

    memset(ptr3, 'A', 16);

    heap_free(ptr2);

    if (heap.capacity != newExpectedCapacity ||
        heap.size != 96 ||
        fList->size != newExpectedCapacity-heap.size ||
        (void*)fList->head != heap.memory || GET_BLOCK_SIZE(fList->head) != 48 || !GET_BLOCK_FREE(fList->head) || fList->head->next == fList->tail ||
        (void*)fList->head->next != (void*)((char*) GET_BLOCK_END_ADDRESS(fList->head) + 48) ||
        GET_BLOCK_SIZE(fList->head->next) != 48 || !GET_BLOCK_FREE(fList->head->next) ||
        (void*)fList->tail != (void*)((char*) GET_BLOCK_END_ADDRESS(fList->head->next) + 48))
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
        (void*)fList->head != heap.memory || GET_BLOCK_SIZE(fList->head) != 144 || !GET_BLOCK_FREE(fList->head) || fList->head->next != NULL)
        res = 1;

    heap_collect();
    return res;
}

int test_heap_free_right_coalesce() {
    int res=0;

    heap_init(160);
    size_t oldCapacity = 192;
    FreeList* fList = &heap.freeList;

    void* ptr = heap_malloc(16);
    void* ptr2 = heap_malloc(16);
    HBlock* incrPtr = expand_heap(64);
    size_t newCapacity = oldCapacity*2;
    HBlock* block = (HBlock*)((char*)ptr - 4);

    heap_free(ptr);

    if (heap.capacity != newCapacity ||
        heap.size != 48 ||
        fList->size != newCapacity-heap.size ||
        (void*)fList->head != heap.memory || GET_BLOCK_SIZE(fList->head) != oldCapacity-(heap.size*2) || !GET_BLOCK_FREE(fList->head) || fList->head->next != fList->tail ||
        fList->tail->prev != fList->head || fList->tail != block ||
            GET_BLOCK_SIZE(block) != oldCapacity+48 || !GET_BLOCK_FREE(block))
        res = 1;

    heap_collect();
    return res;

}

int test_heap_free_all_coalesce() {
    int res=0;

    heap_init(160);
    size_t oldCapacity = 192;
    FreeList* fList = &heap.freeList;

    void* ptr = heap_malloc(16);
    HBlock* incrPtr = expand_heap(64);
    size_t newCapacity = oldCapacity*2;

    heap_free(ptr);

    if (heap.capacity != newCapacity ||
        heap.size != 0 ||
        fList->size != newCapacity ||
        (void*)fList->head != heap.memory || GET_BLOCK_SIZE(fList->head) != newCapacity || !GET_BLOCK_FREE(fList->head) || fList->head->next != NULL || fList->head->prev != NULL ||
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