#include "heep/flist.h"

void list_init(FreeList* list, void* memory, size_t size) {
    list->head = (HBlock*) memory;
    block_init(list->head, size, true, NULL, NULL);
    list->tail = list->head;
    list->size = size;
}

HBlock* list_push_back(FreeList* list, void* memory, size_t size) {
    if (list->head == NULL || list->size == 0) {
        list_init(list, memory, size);
        return list->head;
    }

    HBlock* next = (HBlock*) memory;
    block_init(next, size, true, NULL, list->tail);
    list->tail->next = next;
    list->tail = list->tail->next;
    list->size += GET_BLOCK_SIZE(next);
    return list->tail;
}

void list_pop_front(FreeList* list) {
    HBlock* head = list->head;
    list->size -= GET_BLOCK_SIZE(head);
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
    list->size -= GET_BLOCK_SIZE(tail);
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

    list->size -= GET_BLOCK_SIZE(block);
    block->prev->next = block->next;
    block->next->prev = block->prev;
}

static void list_coalesce(FreeList* list, HBlock* block, HBlock* prev, HBlock* next, size_t coalesceState) {
    switch (coalesceState) {
        case 0:
            block_init(block, GET_BLOCK_SIZE(block), true, next, prev);
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
                nextSize = GET_BLOCK_SIZE(next);
                newNext = next->next;
            }

            block_init(block, GET_BLOCK_SIZE(block) + nextSize, true, newNext, prev);
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
            block_init(prev, GET_BLOCK_SIZE(prev) + GET_BLOCK_SIZE(block), true, next, prev->prev);
            if (next)
                next->prev = prev;
            else 
                list->tail = prev;

            return;

        case 3:
            block_init(prev, GET_BLOCK_SIZE(prev) + GET_BLOCK_SIZE(block) + GET_BLOCK_SIZE(next), true, next->next,
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

void list_insert_and_coalesce(FreeList* list, HBlock* block, const void* start, const void* end) {
    if (list->head == NULL) {
        list_init(list, block, GET_BLOCK_SIZE(block));
        return;
    }

    if ((void*)block == start) {
        list_push_front(list, (void*)start, GET_BLOCK_SIZE(block));
        return;
    }

    HBlock* next = GET_BLOCK_END_ADDRESS((void*)block);
    if ((void*)next == end) {
        list_push_back(list, (void*)start, GET_BLOCK_SIZE(block));
        return;
    }

    HBlock* prev = GET_PREVIOUS_BLOCK_ADDRESS((void*)block);
    size_t coalesceState = (GET_BLOCK_FREE(prev) << 1) | GET_BLOCK_FREE(next);
    switch (coalesceState) {
        case 0:
//           normal insert
            break;
        case 1:
//           coalesce right
            break;
        case 2:
//            coalesce left
            break;
        case 3:
//            coalesce all
            break;
        default:
            printf("[ERROR] Unknown coalesce state!\n");
            exit(1);
    }

    list->size += GET_BLOCK_SIZE(block);

//    if (prev == NULL) {
//        list_coalesce(list, block, NULL, list->head);
//        list->head = block;
//        return;
//    }
//
//    list_coalesce(list, block, prev, prev->next);
}

void list_print(FreeList* list) {
    HBlock* curr = list->head;
    printf("--- Free Blocks State ---\n");
    while (curr != NULL) {
        block_print(curr);
        curr = curr->next;
    }
}