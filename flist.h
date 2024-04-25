
#ifndef FREE_LIST_H
#define FREE_LIST_H

#include "hblock.h"

typedef struct {
    HeaderBlock* head;
    HeaderBlock* tail;
    size_t size;
} FreeList;

void list_init(FreeList* list, void* memory, size_t size);

HeaderBlock* list_append(FreeList* list, void* memory, size_t size);
void list_pop_front(FreeList* list);
void list_pop_back(FreeList* list);
void list_remove(FreeList* list, HeaderBlock* block);

void list_insert_and_coalesce(FreeList* list, HeaderBlock* block);

void list_print(FreeList* list);

#endif