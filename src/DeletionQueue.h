#pragma once

#include <functional>

class DeletionQueue {
    std::vector<std::function<void()>> delete_list;
public:
    DeletionQueue();
    void push(const std::function<void()>&);
    void push(std::function<void()>&&);
    void flush();
};




