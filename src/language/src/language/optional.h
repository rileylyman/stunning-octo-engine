#include <stdint.h>
#include <stdbool.h>

typedef struct {
    bool has_value;
    uint32_t value;
} optional_index_t;

bool has_value(optional_index_t oit);

uint32_t value(optional_index_t oit);

void set_value(optional_index_t *oit, uint32_t value);
