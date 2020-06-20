#pragma once

#include <algorithm>

template <typename T>
struct vector {
    using iterator = T*;
    using const_iterator = T const*;

    vector();                               // O(1) nothrow
    vector(vector const&);                  // O(N) strong
    vector& operator=(vector const& other); // O(N) strong

    ~vector();                              // O(N) nothrow

    T& operator[](size_t i);                // O(1) nothrow
    T const& operator[](size_t i) const;    // O(1) nothrow

    T* data();                              // O(1) nothrow
    T const* data() const;                  // O(1) nothrow
    size_t size() const;                    // O(1) nothrow

    T& front();                             // O(1) nothrow
    T const& front() const;                 // O(1) nothrow

    T& back();                              // O(1) nothrow
    T const& back() const;                  // O(1) nothrow
    void push_back(T const&);               // O(1)* strong
    void pop_back();                        // O(1) nothrow

    bool empty() const;                     // O(1) nothrow

    size_t capacity() const;                // O(1) nothrow
    void reserve(size_t);                   // O(N) strong
    void shrink_to_fit();                   // O(N) strong

    void clear();                           // O(N) nothrow

    void swap(vector&);                     // O(1) nothrow

    iterator begin();                       // O(1) nothrow
    iterator end();                         // O(1) nothrow

    const_iterator begin() const;           // O(1) nothrow
    const_iterator end() const;             // O(1) nothrow

    iterator insert(const_iterator pos, T const&); // O(N) weak

    iterator erase(const_iterator pos);     // O(N) weak

    iterator erase(const_iterator first, const_iterator last); // O(N) weak

private:
    size_t increase_capacity() const;
    void push_back_realloc(T const&);
    void new_buffer(size_t new_capacity);
    static void destroy_all(T* data, size_t size);
    static void copy_construct_all(T* dst, T const* src, size_t size);
    static T* allocate(size_t size);

private:
    T* data_;
    size_t size_;
    size_t capacity_;
};

template<typename T>
vector<T>::vector() {
    data_ = nullptr;
    size_ = 0;
    capacity_ = 0;
}

template<typename T>
vector<T>::vector(vector<T> const& other) {
    T* new_data = allocate(other.size_);

    try {
        copy_construct_all(new_data, other.data_, other.size_);
    } catch (...) {
        operator delete (new_data);
        throw;
    }

    data_ = new_data;
    size_ = other.size_;
    capacity_ = other.capacity_;
}

template<typename T>
vector<T>& vector<T>::operator=(vector<T> const& other) {
    if (this != &other) {
        vector<T> tmp(other);
        swap(tmp);
    }
    return *this;
}

template<typename T>
vector<T>::~vector() {
    destroy_all(data_, size_);
    operator delete(data_);
}

template<typename T>
T& vector<T>::operator[](size_t i) {
    return data_[i];
}

template<typename T>
T const& vector<T>::operator[](size_t i) const {
    return data_[i];
}

template<typename T>
T* vector<T>::data() {
    return data_;
}

template<typename T>
T const* vector<T>::data() const {
    return data_;
}

template<typename T>
size_t vector<T>::size() const {
   return size_;
}

template<typename T>
T& vector<T>::front() {
    return *data_;
}

template<typename T>
T const& vector<T>::front() const {
    return *data_;
}

template<typename T>
T& vector<T>::back() {
    return data_[size_ - 1];
}

template<typename T>
T const& vector<T>::back() const {
    return data_[size_ - 1];
}

template<typename T>
void vector<T>::push_back(const T &a) {
    if (size_ == capacity_) {
        push_back_realloc(a);
    } else {
        new(data_ + size_++) T(a);
    }
}

template<typename T>
void vector<T>::pop_back() {
    data_[--size_].~T();
}

template<typename T>
bool vector<T>::empty() const {
    return size_ == 0;
}

template<typename T>
size_t vector<T>::capacity() const {
    return capacity_;
}

template<typename T>
void vector<T>::reserve(size_t new_capacity) {
    if (new_capacity > capacity_) {
        new_buffer(new_capacity);
    }
}

template<typename T>
void vector<T>::shrink_to_fit() {
    if (size_ < capacity_) {
        new_buffer(size_);
    }
}

template<typename T>
void vector<T>::clear() {
    destroy_all(data_, size_);
    size_ = 0;
}

template<typename T>
void vector<T>::swap(vector &v) {
    std::swap(size_, v.size_);
    std::swap(capacity_, v.capacity_);
    std::swap(data_, v.data_);
}

template<typename T>
typename vector<T>::iterator vector<T>::begin() {
    return data_;
}

template<typename T>
typename vector<T>::iterator vector<T>::end() {
    return data_ + size_;
}

template<typename T>
typename vector<T>::const_iterator vector<T>::begin() const {
    return data_;
}

template<typename T>
typename vector<T>::const_iterator vector<T>::end() const {
    return data_ + size_;
}

template<typename T>
typename vector<T>::iterator vector<T>::insert(vector<T>::const_iterator pos, T const &a) {
    size_t ind = pos - begin();
    push_back(a);

    for (size_t i = size_ - 1; i != ind; i--) {
        std::swap(data_[i], data_[i - 1]);
    }

    return begin() + ind;
}

template <typename T>
typename vector<T>::iterator vector<T>::erase(const_iterator pos) {
    return erase(pos, pos + 1);
}

template <typename T>
typename vector<T>::iterator vector<T>::erase(const_iterator first, const_iterator last) {
    size_t length = last - first;
    size_t ind = first - begin();

    for (size_t i = ind; i < size_ - length; i++) {
        data_[i] = data_[i + length];
    }
    for (size_t i = size_ - length; i < size_; i++) {
        data_[i].~T();
    }
    size_ -= length;

    return begin() + ind;
}

template <typename T>
size_t vector<T>::increase_capacity() const {
    return capacity_ == 0 ? 1 : capacity_ * 2;
}

template <typename T>
void vector<T>::push_back_realloc(T const& a) {
    T b(a);
    new_buffer(increase_capacity());
    new (data_ + size_++) T(b);
}

template <typename T>
void vector<T>::new_buffer(size_t new_capacity) {
    vector<T> tmp;
    if (new_capacity != 0) {
        tmp.data_ = allocate(new_capacity);
        copy_construct_all(tmp.data_, data_, size_);
        tmp.capacity_ = new_capacity;
        tmp.size_ = size_;
    }
    swap(tmp);
}

template <typename T>
void vector<T>::destroy_all(T* data, size_t size) {
    while (size > 0) {
        data[--size].~T();
    }
}

template <typename T>
void vector<T>::copy_construct_all(T* dst, T const* src, size_t size) {
    size_t i = 0;
    try {
        for (; i < size; i++) {
            new (dst + i) T(src[i]);
        }
    } catch (...) {
        destroy_all(dst, i);
        throw;
    }
}

template <typename T>
T* vector<T>::allocate(size_t size) {
    return (size == 0 ? nullptr : static_cast<T*>(operator new(size * sizeof(T))));
}
