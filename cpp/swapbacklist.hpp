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

    void erase(size_t index) {
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

    size_t size() const { return sz; }

    T& operator[](size_t index) { return data[index]; }

    const T& operator[](size_t index) const { return data[index]; }
};

#endif
