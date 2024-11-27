#ifndef DYNARRAY_H
#define DYNARRAY_H

#include <cstdint>

template <typename T>
class DynArray {
    uint16_t size;
    T* data_ptr;

public:
    class iterator {
        size_t current_pos;
        DynArray& array;
    public:
        iterator(DynArray& array, const int pos) : current_pos(pos), array(array) {}

        T& operator*() const {
            return array.data_ptr[current_pos];
        }

        iterator& operator++() {
            ++current_pos;
            return *this;
        }

        iterator& operator--() {
            --current_pos;
            return *this;
        }

        bool operator==(const iterator& other) const {
            return current_pos == other.current_pos;
        }

        bool operator!=(const iterator& other) const {
            return !((*this) == other);
        }
    };

    class const_iterator {
        size_t current_pos;
        const DynArray& array;
    public:
        explicit const_iterator(const DynArray& array, const int pos) : current_pos(pos), array(array) {}

        const T& operator*() const {
            return array.data_ptr[current_pos];
        }

        const_iterator& operator++() {
            ++current_pos;
            return *this;
        }

        const_iterator& operator--() {
            --current_pos;
            return *this;
        }

        bool operator==(const const_iterator& other) const {
            return current_pos == other.current_pos;
        }

        bool operator!=(const const_iterator& other) const {
            return !((*this) == other);
        }
    };

    DynArray();
    explicit DynArray(uint16_t size);
    DynArray(const DynArray& other) noexcept;
    DynArray& operator=(const DynArray& other) noexcept;
    DynArray(DynArray&& other) noexcept;
    ~DynArray();

    [[nodiscard]] bool is_empty() const;
    void resize(uint16_t size);
    [[nodiscard]] uint16_t get_size() const;
    T* data();

    T& operator[](uint16_t index) const;
    T& operator[](uint16_t index);

    iterator begin();
    iterator end();

    const_iterator cbegin() const;
    const_iterator cend() const;
};

template<typename T>
DynArray<T>::DynArray() {
    size = 0;
    data_ptr = nullptr;
}

template<typename T>
DynArray<T>::DynArray(uint16_t size) {
    this->size = size;
    data_ptr = new T[size];
}

template<typename T>
DynArray<T>::DynArray(const DynArray &other) noexcept {
    this->size = other.size;
    this->data_ptr = new T[size];
    memcpy(this->data_ptr, other.data_ptr, this->size * sizeof(T));
}

template<typename T>
DynArray<T>& DynArray<T>::operator=(const DynArray& other) noexcept {
    if (this != &other) {
        delete[] data_ptr;
        this->size = other.size;
        this->data_ptr = new T[size];
        memcpy(this->data_ptr, other.data_ptr, this->size * sizeof(T));
        return *this;
    }
    return *this;
}

template<typename T>
DynArray<T>::DynArray(DynArray&& other) noexcept {
    this->size = other.size;
    this->data_ptr = other.data_ptr;
}

template<typename T>
DynArray<T>::~DynArray() {
    delete[] data_ptr;
}

template<typename T>
bool DynArray<T>::is_empty() const {
    return size == 0;
}

template<typename T>
void DynArray<T>::resize(const uint16_t size) {
    T* new_data = new T[size];
    const T* old_data = data_ptr;
    memcpy(new_data, data_ptr, this->size * sizeof(T));
    data_ptr = new_data;
    this->size = size;
    delete[] old_data;
}

template<typename T>
uint16_t DynArray<T>::get_size() const {
    return size;
}

template<typename T>
T* DynArray<T>::data() {
    return data_ptr;
}

template<typename T>
T& DynArray<T>::operator[](uint16_t index) const {
    return data_ptr[index];
}

template<typename T>
T& DynArray<T>::operator[](uint16_t index) {
    return data_ptr[index];
}

template<typename T>
typename DynArray<T>::iterator DynArray<T>::begin() {
    return DynArray::iterator(*this, 0);
}

template<typename T>
typename DynArray<T>::iterator DynArray<T>::end() {
    return DynArray::iterator(*this, size);
}

template<typename T>
typename DynArray<T>::const_iterator DynArray<T>::cbegin() const {
    return DynArray::const_iterator(*this, 0);
}

template<typename T>
typename DynArray<T>::const_iterator DynArray<T>::cend() const {
    return DynArray::const_iterator(*this, size);
}


#endif
