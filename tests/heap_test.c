//
// Created by prema on 1/5/2024.
//
#include <string.h>

#include "../src/heep/heap.c"

int test_make_heap() {
    make_heap(4096);
    int res=0;
    if (heap.size > 0 ||
        heap.memory == NULL ||
        heap.freeList.size != 4096+32 ||
        heap.freeList.head == NULL ||
        heap.freeList.tail != heap.freeList.head ||
        get_size(heap.freeList.head) != heap.freeList.size ||
        heap.freeList.head->next != NULL ||
        heap.freeList.head->prev != NULL)
    {
        res = 1;
    }

    collect_heap();
    return res;
}

int test_ff_malloc_from_empty() {
    make_heap(64);
    void* ptr = ff_malloc(16);
    FreeList* fList = &heap.freeList;
    HeaderBlock* block = get_end_addr_block(fList->head);
    int res=0;
    if (heap.size != 48 ||
        heap.capacity != 96 ||
        (void*)fList->head != heap.memory ||
        fList->size != 48 ||
        get_size(fList->head) != 48 || !get_free(fList->head) ||
        get_size(block) != 48 || get_free(block) ||
        ptr != (void*)((char*)block + sizeof(uint32_t)))
        res = 1;

    collect_heap();
    return res;
}

int test_ff_malloc_to_full() {
    int res=0;

    make_heap(64);
    void* ptr = ff_malloc(16);
    ptr = ff_malloc(16);

    FreeList* fList = &heap.freeList;
    HeaderBlock* block = heap.memory;

    if (heap.size != 96 ||
        heap.capacity != 96 ||
        (void*)fList->head != NULL || (void*)fList->tail != NULL ||
        fList->size != 0 ||
        get_size(block) != 48 || get_free(block) ||
        ptr != (void*)((char*)block + sizeof(uint32_t)))
        res = 1;

    collect_heap();
    return res;
}

int test_ff_malloc_expand_coalesce() {
    int res=0;

    make_heap(64);
    HeaderBlock* incrPtr = expand_heap(16);
    FreeList* fList = &heap.freeList;

    if (heap.capacity != 128 ||
        heap.size != 0 ||
        (void*)incrPtr != heap.memory ||
        fList->size != 128 ||
        fList->head != incrPtr || fList->tail != incrPtr ||
        fList->head->next != NULL || fList->head->prev != NULL ||
        get_size(fList->head) != 128 || !get_free(fList->head))
        res = 1;

    collect_heap();
    return res;
}

int test_ff_malloc_expand_no_coalesce() {
    int res=0;

    make_heap(64);
    FreeList* fList = &heap.freeList;

    void* ptr = ff_malloc(16);
    HeaderBlock* incrPtr = expand_heap(16);

    if (heap.capacity != 128 ||
        heap.size != 48 ||
        fList->size != 80 ||
        (void*)fList->head != heap.memory || get_size(fList->head) != 48 || !get_free(fList->head) || fList->head->next != fList->tail ||
        incrPtr != fList->tail || fList->tail->prev != fList->head || (void*)fList->tail != (void*)((char*)heap.memory + 96) || get_size(fList->tail) != 32 || !get_free(fList->tail))
        res = 1;

    collect_heap();
    return res;
}

int test_ff_find_fit_order_first() {
    int res=0;

    make_heap(64);
    FreeList* fList = &heap.freeList;

    void* ptr = ff_malloc(16);
    HeaderBlock* incrPtr = expand_heap(16);

    HeaderBlock* victim = ff_find_fit(16);
    if (get_size(victim) < 16 ||
        !get_free(victim) ||
        (void*)victim != heap.memory)
        res = 1;

    collect_heap();
    return res;
}

int test_ff_find_fit_order_last() {
    int res=0;

    make_heap(64);
    FreeList* fList = &heap.freeList;

    void* ptr = ff_malloc(16);
    HeaderBlock* incrPtr = expand_heap(64);

    HeaderBlock* victim = ff_find_fit(64);
    if (get_size(victim) < 64 ||
        !get_free(victim) ||
        (void*)victim != (void*)((char*)heap.memory + 96))
        res = 1;

    collect_heap();
    return res;
}

int test_ff_find_fit_order_none() {
    int res=0;

    make_heap(64);
    FreeList* fList = &heap.freeList;

    void* ptr = ff_malloc(16);
    HeaderBlock* incrPtr = expand_heap(16);

    HeaderBlock* victim = ff_find_fit(64);
    if (victim != NULL)
        res = 1;

    collect_heap();
    return res;
}

int test_ff_free_no_coalesce() {
    int res=0;

    make_heap(160);
    FreeList* fList = &heap.freeList;

    void* ptr = ff_malloc(16);
    HeaderBlock* incrPtr = expand_heap(64);
    void* ptr2 = ff_malloc(16);
    HeaderBlock* block = (HeaderBlock*)((char*)ptr2 - 4);
    void* ptr3 = ff_malloc(16);

    memset(ptr3, 'A', 16);

    ff_free(ptr2);

    if (heap.capacity != 256 ||
        heap.size != 96 ||
        fList->size != 160 ||
        (void*)fList->head != heap.memory || get_size(fList->head) != 48 || !get_free(fList->head) || fList->head->next == fList->tail ||
        (void*)fList->head->next != (void*)((char*)get_end_addr_block(fList->head) + 48) || get_size(fList->head->next) != 48 || !get_free(fList->head->next) ||
        (void*)fList->tail != (void*)((char*) get_end_addr_block(fList->head->next)+48))
        res = 1;

    collect_heap();
    return res;
}

int test_ff_free_left_coalesce() {
    int res=0;

    make_heap(160);
    FreeList* fList = &heap.freeList;

    void* ptr = ff_malloc(16);
    void* ptr2 = ff_malloc(16);
    HeaderBlock* block = (HeaderBlock*)((char*)ptr2 - 4);

    ff_free(ptr2);

    if (heap.capacity != 192 ||
        heap.size != 48 ||
        fList->size != 144 ||
        (void*)fList->head != heap.memory || get_size(fList->head) != 144 || !get_free(fList->head) || fList->head->next != NULL)
        res = 1;

    collect_heap();
    return res;
}

int test_ff_free_right_coalesce() {
    int res=0;

    make_heap(160);
    FreeList* fList = &heap.freeList;

    void* ptr = ff_malloc(16);
    void* ptr2 = ff_malloc(16);
    HeaderBlock* incrPtr = expand_heap(64);
    HeaderBlock* block = (HeaderBlock*)((char*)ptr - 4);

    ff_free(ptr);

    if (heap.capacity != 256 ||
        heap.size != 48 ||
        fList->size != 208 ||
        (void*)fList->head != heap.memory || get_size(fList->head) != 96 || !get_free(fList->head) || fList->head->next != fList->tail ||
        fList->tail->prev != fList->head || fList->tail != block ||
        get_size(block) != 112 || !get_free(block))
        res = 1;

    collect_heap();
    return res;

}

int test_ff_free_all_coalesce() {
    int res=0;

    make_heap(160);
    FreeList* fList = &heap.freeList;

    void* ptr = ff_malloc(16);
    HeaderBlock* incrPtr = expand_heap(64);

    ff_free(ptr);

    if (heap.capacity != 256 ||
        heap.size != 0 ||
        fList->size != 256 ||
        (void*)fList->head != heap.memory || get_size(fList->head) != 256 || !get_free(fList->head) || fList->head->next != NULL || fList->head->prev != NULL ||
        fList->tail != fList->head)
        res = 1;

    collect_heap();
    return res;
}

int main(void) {
    if (test_make_heap())
        return 1;

    if (test_ff_malloc_from_empty())
        return 1;

    if (test_ff_malloc_to_full())
        return 1;

    if (test_ff_malloc_expand_coalesce())
        return 1;

    if (test_ff_malloc_expand_no_coalesce())
        return 1;

    if (test_ff_find_fit_order_first())
        return 1;

    if (test_ff_find_fit_order_last())
        return 1;

    if (test_ff_find_fit_order_none())
        return 1;

    if (test_ff_free_no_coalesce())
        return 1;

    if (test_ff_free_left_coalesce())
        return 1;

    if (test_ff_free_right_coalesce())
        return 1;

    if (test_ff_free_all_coalesce())
        return 1;

    return 0;
}