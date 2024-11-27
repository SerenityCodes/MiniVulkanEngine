#ifndef VULKANDEVICE_H
#define VULKANDEVICE_H


namespace engine {

    class VulkanDevice {
    public:
        struct QueueFamily {
            enum Type {
                COMPUTE,
                TRANSFER,
                GRAPHICS
            };
            uint32_t max_number = ~0;
            Type type = Type::GRAPHICS;
            uint32_t graphics_family = max_number;
            uint32_t present_family = max_number;

            [[nodiscard]] bool is_complete() const;
        };
    private:
        VkResult startup_status = VK_SUCCESS;
        VkPhysicalDevice graphics_device_;
        VkDevice logical_device_;
        VkQueue graphics_queue_;
        VkQueue present_queue_;
        QueueFamily graphics_queue_family_;

    public:

        VulkanDevice(VkInstance instance, VkSurfaceKHR surface);
        ~VulkanDevice();

        [[nodiscard]] VkDevice get_logical_device() const;
        [[nodiscard]] VkPhysicalDevice get_physical_device() const;
        [[nodiscard]] VkQueue get_graphics_queue() const;
        [[nodiscard]] VkQueue get_present_queue() const;
        [[nodiscard]] QueueFamily get_queue_family() const;
        [[nodiscard]] bool was_startup_successful() const;
        [[nodiscard]] VkResult get_startup_status() const;

        operator VkDevice() const;
    };
}

#endif
