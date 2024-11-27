#ifndef CARRAYREF_H
#define CARRAYREF_H

template <typename T>
class CArrayRef<T> {
    const T* data_;
    const int size_;

public:
    CArrayRef(const T* data, const int size) : data_(data), size_(size) { }

    T& operator[](int index) {
        return data_[index];
    }

    [[nodiscard]] int size() const {
        return size_;
    }
};

#endif
