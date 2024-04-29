#ifndef VMEM_H
#define VMEM_H

#include <stdlib.h>

void mem_init();
void* mem_sbrk(size_t increment);
void mem_cleanup();

#endif