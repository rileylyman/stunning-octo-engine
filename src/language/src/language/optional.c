#include "language/optional.h"


struct OptionalIndex optional_index_empty() {
    return (struct OptionalIndex) {
        .has_value = false
    };
}

bool optional_index_has_value(struct OptionalIndex *oit) {
    return oit->has_value;
}

uint32_t optional_index_get_value(struct OptionalIndex *oit) {
    return oit->value;
}

void optional_index_set_value(struct OptionalIndex *oit, uint32_t value) {
    oit->value = value;
    oit->has_value = true;
}
