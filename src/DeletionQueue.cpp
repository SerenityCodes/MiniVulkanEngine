#include "DeletionQueue.h"

DeletionQueue::DeletionQueue() : delete_list() {
}

void DeletionQueue::push(const std::function<void()>& func) {
    delete_list.push_back(func);
}

void DeletionQueue::push(std::function<void()>&& func) {
    delete_list.push_back(std::move(func));
}

void DeletionQueue::flush() {
    for (const auto& func : delete_list) {
        func();
    }
    delete_list.clear();
}
