#pragma once

#include "dynamic_buffer.h"
#include <utility>
#include <cstring>

class opt_vector {
    static const size_t STATIC_CAPACITY = (sizeof(dynamic_buffer*) + 3) / 4;

    size_t size_info;
    union {
        uint32_t static_data[STATIC_CAPACITY];
        dynamic_buffer *dynamic;
    } storage_;

    size_t increase_capacity() const;
    void new_buffer(size_t); //result: unique big
    void unshare();
    void to_big();
    bool is_small() const;
    void delete_buffer();
public:
    opt_vector();
    ~opt_vector();
    opt_vector(const opt_vector&);

    opt_vector &operator=(const opt_vector&);

    size_t size() const;
    uint32_t *data();
    uint32_t &operator[](size_t index);
    uint32_t const &operator[](size_t index) const;
    uint32_t &back();
    uint32_t const &back() const;
    void push_back(uint32_t);
    void pop_back();
    void clear();
    void swap(opt_vector&);
};
