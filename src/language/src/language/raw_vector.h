#pragma once 

#include "log.h"
#include <stdint.h>
#include <stdlib.h>

struct RawVector {
    uint8_t *data;
    size_t   data_size;
    size_t   element_size_in_bytes;
    size_t   count;
};

struct RawVector raw_vector_create(size_t element_size_in_bytes, size_t count);
void raw_vector_destroy(struct RawVector *vector); 

uint8_t *raw_vector_get_ptr(struct RawVector *vector, size_t index);
void raw_vector_set(struct RawVector *vector, size_t index, void *value);
void raw_vector_push_back(struct RawVector *vector, void *value);
void raw_vector_extend_back(struct RawVector *vector, void *value, size_t n);
void raw_vector_pop_back(struct RawVector *vector);
void raw_vector_clear(struct RawVector *vector);
