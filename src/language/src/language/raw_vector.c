#include <memory.h>
#include "raw_vector.h"

//
// Construct a new raw vector from 
//   -  element_size_in_bytes: the size in bytes of an element of this vector
//   -  initial_capacity: the number of elements to allocate space for initially
//
struct RawVector raw_vector_create(size_t element_size_in_bytes, size_t initial_capacity) {
    if (initial_capacity < 1) {
        initial_capacity = 1;
    }
    uint8_t *data = malloc(initial_capacity * element_size_in_bytes);
    if (data == NULL) {
        log_fatal("Could not malloc data for raw_vector\n");
        exit(EXIT_FAILURE);
    }
    return (struct RawVector) {
        .data = malloc(initial_capacity * element_size_in_bytes),
        .data_size = initial_capacity * element_size_in_bytes,
        .element_size_in_bytes = element_size_in_bytes,
        .count = 0
    };
}

//
// Get a pointer to the first byte of the desired element within the raw vector
//
uint8_t *raw_vector_get_ptr(struct RawVector *vector, size_t index) {
    log_trace("Getting index %u from raw_vector\n", index);
    if (index >= vector->count) {
        log_fatal("Attempted to get index of raw_vector greater than its count.\n");
        exit(EXIT_FAILURE);
    }
    return vector->data + index * vector->element_size_in_bytes;
}

//
// Overwrite the element in the vector with the specified value buffer.
// Will overwrite exactly element_size_in_bytes bytes. 
// WARNING: make sure the value buffer contains at least element_size_in_bytes bytes
//
void raw_vector_set(struct RawVector *vector, size_t index, void *value) {
    log_trace("Setting index %u in raw_vector\n", index);
    if (index >= vector->count) {
        log_fatal("Attempted to get index of raw_vector greater than its count.\n");
        exit(EXIT_FAILURE);
    }
    memcpy(vector->data + index * vector->element_size_in_bytes, value, vector->element_size_in_bytes);
}

//
// Resizes the raw vector
//
void resize(struct RawVector *vector, size_t new_size) {
    uint8_t *new_data = realloc(vector->data, new_size);
    if (new_data == NULL) {
        log_fatal("Could not call realloc in vector resize.\n");
        exit(EXIT_FAILURE);
    }
    vector->data = new_data;
    vector->data_size = new_size;
}

//
// Pushes a new element onto the back of the vector.
//
void raw_vector_push_back(struct RawVector *vector, void *value) {
    raw_vector_extend_back(vector, value, 1);
}

//
// Pushes n elements contained in the buffer value onto the back of vector.
//
void raw_vector_extend_back(struct RawVector *vector, void *value, size_t n) {
    size_t desired_size = (vector->count + n) * vector->element_size_in_bytes;
    if (desired_size > vector->data_size) {
        size_t new_size = vector->data_size * 2;
        while (new_size < desired_size) new_size *= 2;
        resize(vector, new_size);
    }
    memcpy(vector->data + vector->count * vector->element_size_in_bytes, value, n * vector->element_size_in_bytes);
    vector->count += n;
}

//
// Pops the back element of the vector.
//
void raw_vector_pop_back(struct RawVector *vector) {
    if (vector->count == 0) {
        log_fatal("Could not pop_back on an empty vector");
        exit(EXIT_FAILURE);
    }
    vector->count -= 1;
    if (vector->count * vector->element_size_in_bytes < vector->data_size / 2) {
        resize(vector, vector->data_size / 2);
    }
}

//
// Clears all data within the vector
//
void raw_vector_clear(struct RawVector *vector) {
    vector->data = realloc(vector->data, 0);
    vector->count = 0;
    vector->data_size = 0;
}

//
// Destroys the vector
//
void raw_vector_destroy(struct RawVector *vector) {
    raw_vector_clear(vector);
}