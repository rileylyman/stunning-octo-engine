#include "language/optional.h"

bool has_value(optional_index_t oit) {
    return oit.has_value;
}

uint32_t value(optional_index_t oit) {
    return oit.value;
}

void set_value(optional_index_t *oit, uint32_t value) {
    oit->value = value;
}
