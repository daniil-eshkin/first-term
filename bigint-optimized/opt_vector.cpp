#include "opt_vector.h"

opt_vector::opt_vector()
    : size_info(1) {} //size 0, small flag true

opt_vector::~opt_vector() {
    if (!is_small()) {
        delete_buffer();
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

uint32_t *opt_vector::data() {
    return (is_small() ? storage_.static_data : storage_.dynamic->data);
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
            storage_.dynamic->data[size()] = a;
        } else {
            storage_.static_data[size()] = a;
        }
        size_info += 2; //size++
        return;
    }
    if (storage_.dynamic->ref_count > 1) {
        new_buffer(size() == storage_.dynamic->capacity ?
                       increase_capacity() : storage_.dynamic->capacity);
    } else if (size() == storage_.dynamic->capacity) {
        new_buffer(increase_capacity());
    }
    storage_.dynamic->data[size()] = a;
    size_info += 2; //size++
}

void opt_vector::pop_back() {
    size_info -= 2; //size--
}

void opt_vector::clear() {
    size_info = 1; //set size to 0 and small flag to true
    delete_buffer();
}

void opt_vector::swap(opt_vector &v) {
    std::swap(size_info, v.size_info);
    std::swap(storage_, v.storage_);
}

size_t opt_vector::increase_capacity() const {
    return storage_.dynamic->capacity == 0 ? 1 : storage_.dynamic->capacity * 2;
}

void opt_vector::new_buffer(size_t new_capacity) { //result: unique big
    dynamic_buffer *new_data = new dynamic_buffer(new_capacity);
    if (size() != 0) {
        memcpy(new_data->data, data(), sizeof(uint32_t) * size());
    }
    if (!is_small()) {
        delete_buffer();
    }
    storage_.dynamic = new_data;
    size_info &= UINT32_MAX - 1; //set small flag to false
}

void opt_vector::unshare() {
    new_buffer(size());
}

void opt_vector::to_big() {
    new_buffer(STATIC_CAPACITY * 2);
}

bool opt_vector::is_small() const {
    return size_info & 1;
}

void opt_vector::delete_buffer() {
    if (--storage_.dynamic->ref_count == 0) {
        delete storage_.dynamic;
    }
}
