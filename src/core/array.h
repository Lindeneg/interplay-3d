#ifndef INTERPLAY_CORE_ARRAY_H_
#define INTERPLAY_CORE_ARRAY_H_

#include <stdint.h>
#include <stdlib.h>

#define array_make(var, type) type *var = (type *)create_array(sizeof(type));

#define array_push(array, value)                      \
    if (array == NULL) {                              \
        (array) = create_array(sizeof(*array));       \
    }                                                 \
    (array) = array_increment(array, sizeof(*array)); \
    (array[array_size(array) - 1]) = value;

#define array_copy(dest, src)                                       \
    {                                                               \
        int num_elements = array_size(src);                         \
        int size = sizeof(*src) * num_elements;                     \
        (dest) = create_array_ex(size, num_elements, num_elements); \
        memcpy(dest, src, size);                                    \
    }

// consider using array_make macro instead
void *create_array(uint64_t element_size);

void *create_array_ex(uint64_t element_size, uint64_t element_count, int size);

// consider using array_push macro instead
void *array_increment(void *array, uint64_t element_size);

int array_size(void *array);

int array_capacity(void *array);

// frees memory block
void array_free(void *array);

// effectively sets size to 0
// whilst capacity remains unchanged
void array_clear(void *array);

#endif  // INTERPLAY_CORE_ARRAY_H_
