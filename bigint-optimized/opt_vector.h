#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <cstring>
#include <memory>

class opt_vector {
    size_t size_;
    size_t capacity_;
    std::shared_ptr<uint32_t> data_;

    size_t increase_capacity() const;
    void new_buffer(size_t);
    uint32_t *allocate(size_t);
    void unshare();
public:
    opt_vector();
    ~opt_vector() = default;
    opt_vector(const opt_vector&);

    opt_vector &operator=(const opt_vector&);

    size_t size() const;
    uint32_t &operator[](size_t index);
    uint32_t const &operator[](size_t index) const;
    uint32_t &back();
    uint32_t const &back() const;
    void push_back(uint32_t);
    void pop_back();
    void clear();
    void swap(opt_vector&);
};
