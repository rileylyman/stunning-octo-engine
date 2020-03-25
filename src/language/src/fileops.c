#include "language/fileops.h"
#include <string.h>
#include "log.h"

//
// Reads bytes of file filename into a malloc'd buffer
// and returns it.
//
uint8_t *read_binary_file_FREE(const char *filename, size_t *size) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        log_fatal("Could not open file %s\n", filename);
        exit(EXIT_FAILURE);
    }
    fseek(file, 0, SEEK_END);
    *size = ftell(file);

    //
    // Make sure that the buffer is aligned to uint32_t
    //
    *size = ((*size + 1) * sizeof(uint32_t)) / sizeof(uint32_t);

    uint8_t *buffer = malloc(sizeof(uint8_t) * *size);
    if (buffer == NULL) {
        log_fatal("Could not malloc buffer!\n");
        exit(EXIT_FAILURE);
    }
    fseek(file, 0, SEEK_SET);
    fread(buffer, sizeof(uint8_t), *size, file);

    return buffer;
}