#include "opt_vector.h"

opt_vector::opt_vector()
    : size_(0)
    , capacity_(0) {}

opt_vector::opt_vector(const opt_vector &v)
    : size_(v.size_)
    , capacity_(v.size_)
    , data_(v.data_) {}

opt_vector &opt_vector::operator=(const opt_vector &v) {
    if (this != &v) {
        opt_vector tmp(v);
        swap(tmp);
    }
    return *this;
}

size_t opt_vector::size() const {
    return size_;
}

uint32_t &opt_vector::operator[](size_t index) {
    if (data_.use_count() > 1) {
        unshare();
    }
    return data_.get()[index];
}

uint32_t const &opt_vector::operator[](size_t index) const {
    return data_.get()[index];
}

uint32_t &opt_vector::back() {
    if (data_.use_count() > 1) {
        unshare();
    }
    return data_.get()[size_ - 1];
}

uint32_t const &opt_vector::back() const {
    return data_.get()[size_ - 1];
}

void opt_vector::push_back(uint32_t a) {
    new_buffer(size_ == capacity_ ? increase_capacity() : capacity_);
    data_.get()[size_++] = a;
}

void opt_vector::pop_back() {
    --size_;
}

void opt_vector::clear() {
    size_ = 0;
    capacity_ = 0;
    data_.reset();
}

void opt_vector::swap(opt_vector &v) {
    std::swap(size_, v.size_);
    std::swap(capacity_, v.capacity_);
    data_.swap(v.data_);
}

size_t opt_vector::increase_capacity() const {
    return capacity_ == 0 ? 1 : capacity_ * 2;
}

void opt_vector::new_buffer(size_t new_capacity) {
    uint32_t *new_data = allocate(new_capacity);
    if (size_ != 0) {
        memcpy(new_data, data_.get(), sizeof(uint32_t) * size_);
    }
    capacity_ = new_capacity;
    data_.reset(new_data);
}

uint32_t *opt_vector::allocate(size_t size) {
    return (size == 0 ? nullptr : static_cast<uint32_t*>(operator new(size * sizeof(uint32_t))));
}

void opt_vector::unshare() {
    new_buffer(size_);
}
