#ifndef VULKAN_ENGINE_H
#define VULKAN_ENGINE_H

#include "GraphicsPipeline.h"
#include "SwapChain.h"
#include "VulkanDevice.h"
#include "VulkanInstance.h"

namespace engine {

    class VulkanEngine {
        VulkanInstance instance_wrapper_;
        VulkanDevice device_wrapper_;
        SwapChain swap_chain_wrapper_;
        GraphicsPipeline graphics_pipeline_;

    public:
        VulkanEngine();
        ~VulkanEngine() = default;

        void run();
        void draw();

        static uint32_t get_file_size(const char* file_name);
        static void read_file(const char* file_name, char* buffer, uint32_t file_size);
    };

}



#endif
