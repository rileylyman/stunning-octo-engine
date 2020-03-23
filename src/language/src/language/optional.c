#include "language/optional.h"
#include "log.h"
#include <stdlib.h>


struct OptionalIndex optional_index_empty() {
    return (struct OptionalIndex) {
        .has_value = false
    };
}

bool optional_index_has_value(struct OptionalIndex *oit) {
    return oit->has_value;
}

uint32_t optional_index_get_value(struct OptionalIndex *oit) {
    if (!oit->has_value) {
        log_fatal("Attempt to get value from optional index without value!\n");
        exit(EXIT_FAILURE);
    }
    return oit->value;
}

void optional_index_set_value(struct OptionalIndex *oit, uint32_t value) {
    oit->value = value;
    oit->has_value = true;
}
