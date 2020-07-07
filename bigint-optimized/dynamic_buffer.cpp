#include "dynamic_buffer.h"

static uint32_t *allocate(size_t size) {
    return (size == 0 ? nullptr : static_cast<uint32_t*>(operator new(size * sizeof(uint32_t))));
}

dynamic_buffer::dynamic_buffer(size_t size)
    : capacity(size)
    , ref_count(1)
    , data(allocate(size)) {}

dynamic_buffer::~dynamic_buffer() {
    operator delete(data);
}
