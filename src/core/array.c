#include "array.h"

#include "../globals.h"
#include "./mem.h"

// offset to store size and capacity
global_internal int context_offset = 2;
// initial array size
global_internal int initial_scale = 16;
// resize scalar, just double the array
global_internal int resize_scale = 2;

/* inspired from:
 * https://github.com/zauonlok/renderer/blob/master/renderer/core/darray.c */
#define ARRAY_RAW_DATA(array) ((int *)(array)-context_offset)
#define ARRAY_CAPACITY(array) (ARRAY_RAW_DATA(array)[0])
#define ARRAY_SIZE(array) (ARRAY_RAW_DATA(array)[1])

void *create_array(uint64_t element_size) {
    return create_array_ex(element_size, initial_scale, 0);
}

void *create_array_ex(uint64_t element_size, uint64_t element_count, int size) {
    uint32_t size_context_bytes = (sizeof(int) * context_offset);
    uint32_t data_bytes = element_size * element_count;
    int *base = (int *)reserve_mem(size_context_bytes + data_bytes);
    /* capacity */
    base[0] = element_count;
    /*  size */
    base[1] = size;
    // offset ptr to actual data
    return base + context_offset;
}

void *array_increment(void *array, uint64_t element_size) {
    int size = array_size(array);
    int capacity = array_capacity(array);
    if (size + 1 >= capacity) {
        int new_capacity = capacity * resize_scale;
        uint32_t double_size =
            (sizeof(int) * context_offset) + (element_size * new_capacity);
        int *base = (int *)realloc_mem(ARRAY_RAW_DATA(array), double_size);
        base[0] = new_capacity;
        base[1] = size + 1;
        return base + context_offset;
    }
    ARRAY_SIZE(array) += 1;
    return array;
}

int array_size(void *array) { return (array != NULL) ? ARRAY_SIZE(array) : 0; }

int array_capacity(void *array) {
    return (array != NULL) ? ARRAY_CAPACITY(array) : 0;
}

void array_free(void *array) {
    if (array != NULL) {
        free_mem(ARRAY_RAW_DATA(array));
    }
}

void array_clear(void *array) {
    if (array != NULL) {
        ARRAY_SIZE(array) = 0;
    }
}

