#include "language/optional.h"


struct OptionalIndex optional_index_empty() {
    return (struct OptionalIndex) {
        .has_value = false
    };
}

bool has_value(struct OptionalIndex *oit) {
    return oit->has_value;
}

uint32_t value(struct OptionalIndex *oit) {
    return oit->value;
}

void set_value(struct OptionalIndex *oit, uint32_t value) {
    oit->value = value;
    oit->has_value = true;
}
