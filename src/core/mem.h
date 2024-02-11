#ifndef INTERPLAY_CORE_MEM_H_
#define INTERPLAY_CORE_MEM_H_

#include <stdint.h>

void *reserve_mem(uint64_t size);

void *reserve_cmem(uint64_t elements, uint64_t size);

void *realloc_mem(void *ptr, uint64_t new_size);

void free_mem(void *ptr);

#endif  // INTERPLAY_CORE_MEM_H_
