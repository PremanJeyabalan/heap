//
// Created by prema on 1/5/2024.
//
#include "../src/heep/flist.c"
#include "heep/hblock.h"


int test_list_init() {
    char memory[64] = {0};
    FreeList fList;
    list_init(&fList, (void*)memory, 64);
    size_t listSize = fList.size;
    bool listPtrCheck = fList.head == fList.tail;
    size_t headSize = block_get_size(fList.head);
    bool headFreeCheck = block_get_free(fList.head);

    if (listSize != 64 || !listPtrCheck || headSize != 64 || !headFreeCheck) {
        printf("Failed FreeList Init Check! list size Expected: %lu size Seen: %lu head seen: %p tail seen: %p head size seen: %lu head free value seen: %d\n",
               (size_t)64, listSize, (void*)fList.head, (void*)fList.tail, headSize, headFreeCheck);
        return 1;
    }

    return 0;
}

int test_list_append_empty() {
    char memory[64] = {0};
    FreeList fList = {0};
    HBlock* head = list_append(&fList, (void*)memory, 64);
    if ((void*)head != memory || !block_get_free(head) || block_get_size(head) != 64 || fList.size != 64 || fList.head != fList.tail)
        return 1;

    return 0;
}

int test_list_append_single() {
    char memory[128] = {0};
    FreeList fList = {0};
    HBlock* head = list_append(&fList, (void*)memory, 64);
    HBlock* block = list_append(&fList, block_get_end_addr(head), 64);
    if (
            (void*)block != (void*)((char*)fList.head + 64) ||
            fList.head->next != block ||
            block->prev != fList.head ||
            block->next != NULL ||
                    block_get_size(block) != 64 ||
            block != fList.tail ||
            fList.size != 128)
        return 1;

    return 0;
}

int test_list_append_double() {
    char memory[160] = {0};
    FreeList fList = {0};
    HBlock* head = list_append(&fList, (void*)memory, 64);
    HBlock* b = list_append(&fList, block_get_end_addr(head), 64);
    HBlock* block = list_append(&fList, block_get_end_addr(b), 32);

    if (
            (void*)block != (void*)((char*)b + 64) ||
            b->next != block ||
            block->prev != b ||
            block->next != NULL ||
                    block_get_size(block) != 32 ||
            block != fList.tail ||
            fList.size != 160)
        return 1;

    return 0;
}

int test_list_pop_front_single() {
    char memory[64] = {0};
    FreeList fList;
    list_init(&fList, (void*)memory, 64);
    list_pop_front(&fList);
    if (fList.size != 0 ||
        fList.head != NULL ||
        fList.tail != NULL)
        return 1;

    return 0;
};

int test_list_pop_front_double() {
    char memory[128] = {0};
    FreeList fList = {0};
    HBlock* head = list_append(&fList, (void*)memory, 64);
    HBlock* block = list_append(&fList, block_get_end_addr(head), 64);

    list_pop_front(&fList);
    if (fList.size != 64 ||
        fList.head != block ||
        block->prev != NULL ||
        block->next != NULL ||
        fList.tail != block)
        return 1;

    return 0;
}

int test_list_pop_back_single() {
    char memory[64] = {0};
    FreeList fList;
    list_init(&fList, (void*)memory, 64);
    list_pop_back(&fList);
    if (fList.size != 0 ||
        fList.head != NULL ||
        fList.tail != NULL)
        return 1;

    return 0;

};

int test_list_pop_back_double() {
    char memory[128] = {0};
    FreeList fList = {0};
    HBlock* head = list_append(&fList, (void*)memory, 64);
    HBlock* block = list_append(&fList, block_get_end_addr(head), 64);

    list_pop_back(&fList);
    if (fList.size != 64 ||
        fList.head != head ||
        head->prev != NULL ||
        head->next != NULL ||
        fList.tail != head)
        return 1;

    return 0;

}

int test_list_remove_head() {
    char memory[128] = {0};
    FreeList fList = {0};
    HBlock* head = list_append(&fList, (void*)memory, 64);
    HBlock* block = list_append(&fList, block_get_end_addr(head), 64);

    list_remove(&fList, head);
    if (fList.size != 64 ||
        fList.head != block ||
        block->prev != NULL ||
        block->next != NULL ||
        fList.tail != block)
        return 1;

    return 0;
}

int test_list_remove_tail() {
    char memory[128] = {0};
    FreeList fList = {0};
    HBlock* head = list_append(&fList, (void*)memory, 64);
    HBlock* block = list_append(&fList, block_get_end_addr(head), 64);

    list_remove(&fList, block);

    if (fList.size != 64 ||
        fList.head != head ||
        head->prev != NULL ||
        head->next != NULL ||
        fList.tail != head)
        return 1;

    return 0;
}

int test_list_remove_large() {
    char memory[160] = {0};
    FreeList fList = {0};
    HBlock* head = list_append(&fList, (void*)memory, 64);
    HBlock* b = list_append(&fList, block_get_end_addr(head), 64);
    HBlock* block = list_append(&fList, block_get_end_addr(b), 32);

    list_remove(&fList, b);

    if (fList.size != 96 ||
        fList.head != head ||
        fList.tail != block ||
        head->next != block ||
        block->prev != head)
        return 1;

    return 0;
}

int test_list_find_prev_head() {
    char memory[160] = {0};
    void* start = (void*)((char*)memory+32);
    FreeList fList = {0};
    HBlock* head = list_append(&fList, (void*)start, 64);
    HBlock* b = list_append(&fList, block_get_end_addr(head), 64);

    HBlock* block = (HBlock*)memory;
    block_init(block, 32, true, NULL, NULL);

    HBlock* prev = list_find_prev(&fList, block);
    if (prev != NULL)
        return 1;

    return 0;
}

int test_list_find_prev_tail() {
    char memory[160] = {0};
    FreeList fList = {0};
    HBlock* head = list_append(&fList, memory, 64);
    HBlock* b = list_append(&fList, block_get_end_addr(head), 64);

    HBlock* block = (HBlock*)((char*)memory + 128);
    block_init(block, 32, true, NULL, NULL);

    HBlock* prev = list_find_prev(&fList, block);
    if (prev != b)
        return 1;

    return 0;
};

int test_list_find_prev_middle() {
    char memory[160] = {0};
    FreeList fList = {0};
    HBlock* head = list_append(&fList, (void*)memory, 64);
    HBlock* b = list_append(&fList, block_get_end_addr(head), 64);
    HBlock* block = list_append(&fList, block_get_end_addr(b), 32);
    list_remove(&fList, b);

    HBlock* prev = list_find_prev(&fList, b);
    if (prev != head)
        return 1;

    return 0;
}

int test_list_insert_and_coalesce_none() {
    char memory[160] = {0};
    FreeList fList = {0};
    HBlock* head = list_append(&fList, (void*)memory, 64);
    HBlock* b = list_append(&fList, block_get_end_addr(head), 32);
    HBlock* block = list_append(&fList, block_get_end_addr(b), 64);
    list_remove(&fList, block);
    block->next = NULL;
    block->prev = NULL;

    list_remove(&fList, b);
    b->next = NULL;
    b->prev = NULL;



    list_insert_and_coalesce(&fList, block);
    if (fList.size != 128 ||
        fList.head != head ||
            block_get_size(head) != 64 ||
            block_get_size(block) != 64 ||
        head->next != block ||
        block->prev != head)
        return 1;

    return 0;
}

int test_list_insert_and_coalesce_left() {
    char memory[1600] = {0};
    FreeList fList = {0};
    HBlock* head = list_append(&fList, (void*)memory, 64);
    HBlock* block = list_append(&fList, (void*)((char*) block_get_end_addr(head) + 64), 64);

    HBlock* b = block_get_end_addr(head);
    block_init(b, 32, true, NULL, NULL);

    list_insert_and_coalesce(&fList, b);
    if (fList.size != 160 ||
        fList.head != head ||
        fList.tail != block ||
        head->next != block ||
        block->prev != head ||
            block_get_size(head) != 96 ||
            block_get_size(block) != 64)
        return 1;

    return 0;
}

int test_list_insert_and_coalesce_right() {
    char memory[1600] = {0};
    FreeList fList = {0};
    HBlock* head = list_append(&fList, (void*)memory, 64);
    HBlock* block = list_append(&fList, (void*)((char*) block_get_end_addr(head) + 64), 64);

    HBlock* b = (void*)((char*) block_get_end_addr(head) + 32);
    block_init(b, 32, true, NULL, NULL);

    list_insert_and_coalesce(&fList, b);
    if (fList.size != 160 ||
        fList.head != head ||
        fList.tail != b ||
        head->next != b ||
        b->prev != head ||
            block_get_size(head) != 64 ||
            block_get_size(b) != 96)
        return 1;

    return 0;
}

int test_list_insert_and_coalesce_all() {
    char memory[1600] = {0};
    FreeList fList = {0};
    HBlock* head = list_append(&fList, (void*)memory, 64);
    HBlock* block = list_append(&fList, (void*)((char*) block_get_end_addr(head) + 64), 64);

    HBlock* b = block_get_end_addr(head);
    block_init(b, 64, true, NULL, NULL);

    list_insert_and_coalesce(&fList, b);
    if (fList.size != 192 ||
        fList.head != head ||
        fList.tail != head ||
        head->next != NULL ||
        head->prev != NULL ||
            block_get_size(head) != 192)
        return 1;

    return 0;
}

int main(void) {
    if (test_list_init())
        return 1;

    if (test_list_append_empty())
        return 1;

    if (test_list_append_single())
        return 1;

    if (test_list_append_double())
        return 1;

    if (test_list_pop_front_single())
        return 1;

    if (test_list_pop_front_double())
        return 1;

    if (test_list_pop_back_single())
        return 1;

    if (test_list_pop_back_double())
        return 1;

    if (test_list_remove_head())
        return 1;

    if (test_list_remove_tail())
        return 1;

    if (test_list_remove_large())
        return 1;

    if (test_list_find_prev_head())
        return 1;

    if (test_list_find_prev_tail())
        return 1;

    if (test_list_find_prev_middle())
        return 1;

    if (test_list_insert_and_coalesce_none())
        return 1;

    if (test_list_insert_and_coalesce_left())
        return 1;

    if (test_list_insert_and_coalesce_right())
        return 1;

    if (test_list_insert_and_coalesce_all())
        return 1;

    return 0;
}