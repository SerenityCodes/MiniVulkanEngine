#include "StackAllocator.h"

#include "fmt/format.h"

namespace engine {

    StackAllocator::StackAllocator(const uint32_t stack_size) {
        this->stack_size = stack_size;
        stack_start = new uint32_t[stack_size];
        stack_ptr = stack_start;
    }

    StackAllocator::~StackAllocator() {
        delete[] stack_start;
    }

    void* StackAllocator::allocate(uint32_t size) {
        if (stack_ptr + size > stack_start + stack_size) {
            return nullptr;
        }
        return allocate_unchecked(size);
    }

    void* StackAllocator::allocate_unchecked(const uint32_t size) {
        void* ptr = stack_ptr;
        stack_ptr += size;
        return ptr;
    }

    void StackAllocator::free_to_marker(uint32_t* ptr) {
        if (ptr < stack_ptr || ptr >= stack_ptr + stack_size) {
            return;
        }
        free_unchecked(ptr);
    }

    void StackAllocator::free_unchecked(uint32_t *ptr) {
        stack_ptr = ptr;
    }
}
