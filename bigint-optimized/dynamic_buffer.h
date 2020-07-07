#pragma once

#include <cstdint>
#include <cstddef>

struct dynamic_buffer {
    size_t capacity;
    size_t ref_count;
    uint32_t *data;
    dynamic_buffer(size_t);

    ~dynamic_buffer();
};
