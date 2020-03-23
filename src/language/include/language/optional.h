#pragma once

#include <stdint.h>
#include <stdbool.h>

struct OptionalIndex {
    bool has_value;
    uint32_t value;
};

struct OptionalIndex optional_index_empty();

bool optional_index_has_value(struct OptionalIndex *oit);

uint32_t optional_index_get_value(struct OptionalIndex *oit);

void optional_index_set_value(struct OptionalIndex *oit, uint32_t value);
