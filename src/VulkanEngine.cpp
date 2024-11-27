#include "VulkanEngine.h"

#include <fstream>
#include <iostream>
#include <SDL.h>

#include "VulkanInstance.h"

void* operator new(size_t size) {
    std::cout << "Allocated " << size << " bytes\n";
    return malloc(size);
}

namespace engine {

    VulkanEngine::VulkanEngine() : instance_wrapper_(1200, 800),
        device_wrapper_(instance_wrapper_.get_vk_instance(), instance_wrapper_.get_surface_surface()),
        swap_chain_wrapper_(device_wrapper_, instance_wrapper_.get_surface_surface(), instance_wrapper_.get_raw_window()),
        graphics_pipeline_(device_wrapper_, swap_chain_wrapper_, device_wrapper_.get_queue_family()) {

    }

    void VulkanEngine::run() {
        SDL_Event event;
        while (true) {
            SDL_PollEvent(&event);
            if (event.type == SDL_QUIT) {
                break;
            }
            draw();
        }
    }

    void VulkanEngine::draw() {
        graphics_pipeline_.draw();
    }

    uint32_t VulkanEngine::get_file_size(const char* file_name) {
        FILE* file = fopen(file_name, "rb");
        if (file == nullptr) {
            return 0;
        }
        fseek(file, 0, SEEK_END);
        const long len = ftell(file);
        fclose(file);
        return len;
    }

    void VulkanEngine::read_file(const char* file_name, char* buffer, const uint32_t file_size) {
        FILE* file = fopen(file_name, "rb");
        if (file == nullptr) {
            return;
        }
        fread(buffer, file_size, 1, file);
        fclose(file);
        buffer[file_size] = 0;
    }
}
