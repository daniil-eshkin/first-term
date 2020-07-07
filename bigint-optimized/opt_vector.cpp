#include "opt_vector.h"

opt_vector::opt_vector()
    : size_info(1) {}

opt_vector::~opt_vector() {
    if (!is_small()) {
        if (--storage_.dynamic->ref_count == 0) {
            delete storage_.dynamic;
        }
    }
}

opt_vector::opt_vector(const opt_vector &v)
    : size_info(v.size_info)
{
    if (is_small()) {
        memcpy(storage_.static_data, v.storage_.static_data, sizeof(uint32_t) * STATIC_CAPACITY);
    } else {
        storage_.dynamic = v.storage_.dynamic;
        storage_.dynamic->ref_count++;
    }
}

opt_vector &opt_vector::operator=(const opt_vector &v) {
    if (this != &v) {
        opt_vector tmp(v);
        swap(tmp);
    }
    return *this;
}

size_t opt_vector::size() const {
    return size_info >> 1;
}

uint32_t &opt_vector::operator[](size_t index) {
    if (is_small()) {
        return storage_.static_data[index];
    }
    if (storage_.dynamic->ref_count > 1) {
        unshare();
    }
    return storage_.dynamic->data[index];
}

uint32_t const &opt_vector::operator[](size_t index) const {
    if (is_small()) {
        return storage_.static_data[index];
    }
    return storage_.dynamic->data[index];
}

uint32_t &opt_vector::back() {
    return (*this)[size() - 1];
}

uint32_t const &opt_vector::back() const {
    return (*this)[size() - 1];
}

void opt_vector::push_back(uint32_t a) {
    if (is_small()) {
        if (size() == STATIC_CAPACITY) {
            to_big();
        } else {
            storage_.static_data[size()] = a;
            size_info += 2;
            return;
        }
    }
    if (storage_.dynamic->ref_count > 1) {
        new_buffer(size() == storage_.dynamic->capacity ?
                       increase_capacity() : storage_.dynamic->capacity);
    } else if (size() == storage_.dynamic->capacity) {
        new_buffer(increase_capacity());
    }
    storage_.dynamic->data[size()] = a;
    size_info += 2;
}

void opt_vector::pop_back() {
    size_info -= 2;
}

void opt_vector::clear() {
    size_info = 1;
    if (--storage_.dynamic->ref_count == 0) {
        delete storage_.dynamic;
    }
}

void opt_vector::swap(opt_vector &v) {
    std::swap(size_info, v.size_info);
    std::swap(storage_, v.storage_);
}

size_t opt_vector::increase_capacity() const {
    return storage_.dynamic->capacity == 0 ? 1 : storage_.dynamic->capacity * 2;
}

void opt_vector::new_buffer(size_t new_capacity) {
    dynamic_buffer *new_data = new dynamic_buffer;
    new_data->capacity = new_capacity;
    new_data->ref_count = 1;
    new_data->data = allocate(new_capacity);
    if (size() != 0) {
        memcpy(new_data->data, storage_.dynamic->data, sizeof(uint32_t) * size());
    }
    if (--storage_.dynamic->ref_count == 0) {
        delete storage_.dynamic;
    }
    storage_.dynamic = new_data;
}

uint32_t *opt_vector::allocate(size_t size) {
    return (size == 0 ? nullptr : reinterpret_cast<uint32_t*>(operator new(size * sizeof(uint32_t))));
}

void opt_vector::unshare() {
    new_buffer(size());
}

void opt_vector::to_big() {
    uint32_t a0 = storage_.static_data[0];
    uint32_t a1 = storage_.static_data[1];
    dynamic_buffer *new_data = new dynamic_buffer;
    new_data->capacity = STATIC_CAPACITY;
    new_data->ref_count = 1;
    new_data->data = allocate(STATIC_CAPACITY);
    storage_.dynamic = new_data;
    storage_.dynamic->data[0] = a0;
    storage_.dynamic->data[1] = a1;
    size_info ^= 1;
}

bool opt_vector::is_small() const {
    return size_info & 1;
}
