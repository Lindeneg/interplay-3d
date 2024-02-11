#include "mem.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../log.h"

void *reserve_mem(uint64_t size) {
    void *ptr = malloc(size);
    assert(ptr != NULL);
    log_verbose("allocated %u bytes to address %p", size, ptr);
    return ptr;
}

void *reserve_cmem(uint64_t elements, uint64_t size) {
    void *ptr = calloc(elements, size);
    assert(ptr != NULL);
    log_verbose(
        "allocated %u bytes to address %p and initialized %u "
        "elements",
        size, ptr, elements);
    return ptr;
}

void *realloc_mem(void *ptr, uint64_t new_size) {
    void *new_ptr = realloc(ptr, new_size);
    assert(new_ptr != NULL);
    log_verbose("reallocated %u bytes from address %p to address %p", new_size,
                ptr, new_ptr);
    return new_ptr;
}

void free_mem(void *ptr) {
    assert(ptr != NULL);
    log_verbose("freeing address %p", ptr);
    free(ptr);
}

