#ifndef _SWAPBACKLIST_HPP
#define _SWAPBACKLIST_HPP

#include <cstddef>

template <typename T> class SwapBackList {
private:
    T* data;
    size_t capacity;
    size_t sz;

public:
    SwapBackList(size_t initial_capacity = 10) : capacity(initial_capacity), sz(0) { data = new T[capacity]; }

    ~SwapBackList() { delete[] data; }

    void remove(size_t index) {
        if (index < sz) {
            data[index] = data[--sz];
        }
    }

    void push_back(const T& value) {
        if (sz >= capacity) {
            capacity *= 2;
            T* new_data = new T[capacity];
            for (size_t i = 0; i < sz; ++i) {
                new_data[i] = data[i];
            }
            delete[] data;
            data = new_data;
        }
        data[sz++] = value;
    }

    void insert(const T& value) { push_back(value); }

    size_t size() const { return sz; }

    T& operator[](size_t index) { return data[index]; }

    const T& operator[](size_t index) const { return data[index]; }

    bool contains(const T& value) const {
        for (size_t i = 0; i < sz; ++i) {
            if (data[i] == value) {
                return true;
            }
        }
        return false;
    }

    // iterators
    using iterator = T*;
    using const_iterator = const T*;
    iterator begin() { return data; }
    iterator end() { return data + sz; }
    const_iterator begin() const { return data; }
    const_iterator end() const { return data + sz; }
    const_iterator cbegin() const { return data; }
    const_iterator cend() const { return data + sz; }
};

#endif
