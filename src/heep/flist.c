#include "heep/flist.h"

void list_init(FreeList* list, void* memory, size_t size) {
    list->head = (HBlock*) memory;
    block_init(list->head, size, true, NULL, NULL);
    list->tail = list->head;
    list->size = size;
}

HBlock* list_append(FreeList* list, void* start, size_t size) {
    if (list->head == NULL || list->size == 0) {
        list_init(list, start, size);
        return list->head;
    }

    HBlock* next = (HBlock*) start;
    block_init(next, size, true, NULL, list->tail);
    list->tail->next = next;
    list->tail = list->tail->next;
    list->size += block_get_size(next);
    return list->tail;
}

void list_pop_front(FreeList* list) {
    HBlock* head = list->head;
    list->size -= block_get_size(head);
    if (head->next == NULL) {
        list->head = NULL;
        list->tail = NULL;
        return;
    }

    list->head = list->head->next;
    list->head->prev = NULL;
}

void list_pop_back(FreeList* list) {
    HBlock* tail = list->tail;
    list->size -= block_get_size(tail);
    if (list->tail == list->head) {
        list->head = NULL;
        list->tail = NULL;
        return;
    }

    list->tail->prev->next = NULL;
    list->tail = list->tail->prev;
}

void list_remove(FreeList* list, HBlock* block) {
    if (block == list->head)  {
        list_pop_front(list);
        return;
    }

    if (block == list->tail) {
        list_pop_back(list);
        return;
    }

    list->size -= block_get_size(block);
    block->prev->next = block->next;
    block->next->prev = block->prev;
}

static HBlock* list_find_prev(FreeList* list, const void* block) {
    void* curr = (void*)list->head;
    if ((void*)list->tail < block) {
        // printf("[FREE] Tail: %p is smaller than block %p\n", (void*)list->tail, block);
        return ((HBlock*) list->tail);
    }

    while (curr != NULL) {
        if (curr > block)
            return ((HBlock*) curr)->prev;

        curr = (void*) ((HBlock*) curr)->next;
    }
    
    return NULL;
}

static void list_coalesce(FreeList* list, HBlock* block, HBlock* prev, HBlock* next) {
    size_t coalesceState = 0;
    coalesceState |= (size_t)(prev && (block_get_end_addr(prev) == (void*) block));
    coalesceState <<= 1;
    coalesceState |= (size_t)(next && (block_get_end_addr(block) == (void*) next));
    // printf("[FREE] Coalesce state: %lu\n", coalesceState);

    switch (coalesceState) {
        case 0:
            block_init(block, block_get_size(block), true, next, prev);
            if (prev)
                prev->next = block;
            else 
                list->head = block;
            
            if (next)
                next->prev = block;
            else 
                list->tail = block;

            return;

        case 1: {
            size_t nextSize = 0;
            HBlock* newNext = NULL;
            if (next) {
                nextSize = block_get_size(next);
                newNext = next->next;
            }

            block_init(block, block_get_size(block) + nextSize, true, newNext, prev);
            if (prev)
                prev->next = block;
            else 
                list->head = block;

            if (newNext)
                newNext->prev = block;
            else
                list->tail = block;

            return;
        }

        case 2:
            block_init(prev, block_get_size(prev) + block_get_size(block), true, next, prev->prev);
            if (next)
                next->prev = prev;
            else 
                list->tail = prev;

            return;

        case 3:
            block_init(prev, block_get_size(prev) + block_get_size(block) + block_get_size(next), true, next->next,
                       prev->prev);
            if (next->next)
                next->next->prev = prev;
            else
                list->tail = prev;

            return;

        default:
            printf("[ERROR] Coalesce has invalid state: %lu\n", coalesceState);
            exit(1);
    }

}

void list_insert_and_coalesce(FreeList* list, HBlock* block) {
    if (list->head == NULL) {
        list_init(list, block, block_get_size(block));
        return;
    }

    list->size += block_get_size(block);

    HBlock* prev = list_find_prev(list, block);
    if (prev == NULL) {
        // printf("[FREE] Trying to add %p to front of list starting %p\n", (void*) block, (void*) list->head);
        list_coalesce(list, block, NULL, list->head);
        list->head = block;
        return;
    }

    list_coalesce(list, block, prev, prev->next);
}

void list_print(FreeList* list) {
    HBlock* curr = list->head;
    printf("--- Free Blocks State ---\n");
    while (curr != NULL) {
        block_print(curr);
        curr = curr->next;
    }
}