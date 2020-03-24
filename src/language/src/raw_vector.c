#include <memory.h>
#include "language/raw_vector.h"

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
        .data_size_in_bytes = initial_capacity * element_size_in_bytes,
        .element_size_in_bytes = element_size_in_bytes,
        .count_in_elements = 0
    };
}

//
// Get the number of elements contained in this vector.
//
size_t raw_vector_size(struct RawVector *vector) {
    if (vector->data == NULL) {
        log_fatal("Cannot get size on destroyed vector\n");
        exit(EXIT_FAILURE);
    }
    return vector->count_in_elements;
}

//
// Get a pointer to the first byte of the desired element within the raw vector
//
uint8_t *raw_vector_get_ptr(struct RawVector *vector, size_t index) {
    if (vector->data == NULL) {
        log_fatal("Cannot get index on destroyed vector\n");
        exit(EXIT_FAILURE);
    }
    log_trace("Getting index %u from raw_vector\n", index);
    if (index >= vector->count_in_elements) {
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
    if (vector->data == NULL) {
        log_fatal("Cannot set index on destroyed vector\n");
        exit(EXIT_FAILURE);
    }
    log_trace("Setting index %u in raw_vector\n", index);
    if (index >= vector->count_in_elements) {
        log_fatal("Attempted to get index of raw_vector greater than its count.\n");
        exit(EXIT_FAILURE);
    }
    memcpy(vector->data + index * vector->element_size_in_bytes, value, vector->element_size_in_bytes);
}

//
// Resizes the raw vector
//
void resize(struct RawVector *vector, size_t new_size) {
    if (vector->data == NULL) {
        log_fatal("Cannot resize on destroyed vector\n");
        exit(EXIT_FAILURE);
    }
    uint8_t *new_data = realloc(vector->data, new_size);
    if (new_data == NULL) {
        log_fatal("Could not call realloc in vector resize.\n");
        exit(EXIT_FAILURE);
    }
    vector->data = new_data;
    vector->data_size_in_bytes = new_size;
}

//
// Pushes a new element onto the back of the vector.
//
void raw_vector_push_back(struct RawVector *vector, void *value) {
    if (vector->data == NULL) {
        log_fatal("Cannot push back on destroyed vector\n");
        exit(EXIT_FAILURE);
    }
    raw_vector_extend_back(vector, value, 1);
}

//
// Pushes n elements contained in the buffer value onto the back of vector.
//
void raw_vector_extend_back(struct RawVector *vector, void *value, size_t n) {
    if (vector->data == NULL) {
        log_fatal("Cannot extend on destroyed vector\n");
        exit(EXIT_FAILURE);
    }
    size_t desired_size = (vector->count_in_elements + n) * vector->element_size_in_bytes;
    if (desired_size > vector->data_size_in_bytes) {
        size_t new_size = vector->data_size_in_bytes * 2;
        while (new_size < desired_size) new_size *= 2;
        resize(vector, new_size);
    }
    memcpy(vector->data + vector->count_in_elements * vector->element_size_in_bytes, value, n * vector->element_size_in_bytes);
    vector->count_in_elements += n;
}

//
// Pops the back element of the vector.
//
void raw_vector_pop_back(struct RawVector *vector) {
    if (vector->data == NULL) {
        log_fatal("Cannot pop back on destroyed vector\n");
        exit(EXIT_FAILURE);
    }
    if (vector->count_in_elements == 0) {
        log_fatal("Could not pop_back on an empty vector");
        exit(EXIT_FAILURE);
    }
    vector->count_in_elements -= 1;
    if (vector->count_in_elements * vector->element_size_in_bytes < vector->data_size_in_bytes / 2) {
        resize(vector, vector->data_size_in_bytes / 2);
    }
}

//
// Clears all data within the vector
//
void raw_vector_clear(struct RawVector *vector) {
    if (vector->data == NULL) {
        log_fatal("Cannot clear a destroyed vector!\n");
        exit(EXIT_FAILURE);
    }
    vector->data = realloc(vector->data, vector->element_size_in_bytes);
    vector->count_in_elements = 0;
    vector->data_size_in_bytes = vector->element_size_in_bytes;
}

//
// Destroys the vector
//
void raw_vector_destroy(struct RawVector *vector) {
    free(vector->data);
    vector->data = NULL;
    vector->count_in_elements = 0;
}