#include "VulkanDevice.h"

#include <cstring>
#include <vulkan/vk_enum_string_helper.h>

#include "SwapChain.h"

namespace engine {

    const char* NEEDED_EXTENSIONS[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    constexpr int NEEDED_EXTENSION_COUNT = 1;

    bool isDeviceSuitable(VkPhysicalDevice device) {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
               deviceFeatures.geometryShader;
    }

    bool check_device_support(VkPhysicalDevice physical_device) {
        uint32_t extension_count = 0;
        vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, nullptr);

        VkExtensionProperties extensions[extension_count];
        vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, extensions);

        bool extensions_supported[NEEDED_EXTENSION_COUNT]{};
        for (uint32_t i = 0; i < extension_count; i++) {
            VkExtensionProperties extension = extensions[i];
            for (int j = 0; j < NEEDED_EXTENSION_COUNT; j++) {
                if (strcmp(extension.extensionName, NEEDED_EXTENSIONS[j]) == 0) {
                    extensions_supported[j] = true;
                }
            }
        }
        bool all_true = true;
        for (const bool i : extensions_supported) {
            if (!i) {
                all_true = false;
            }
        }
        return all_true;
    }

    VulkanDevice::QueueFamily find_queue_family(VkPhysicalDevice physical_device, VkSurfaceKHR surface) {
        VulkanDevice::QueueFamily family{};
        uint32_t queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);

        VkQueueFamilyProperties queue_families[queue_family_count];
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families);

        for (int i = 0; i < queue_family_count; i++) {
            VkQueueFamilyProperties properties = queue_families[i];
            if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                family.graphics_family = i;
            }
            VkBool32 present_support = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &present_support);
            if (present_support) {
                family.present_family = i;
            }
            if (family.is_complete()) {
                break;
            }
        }

        return family;
    }

    bool is_swap_chain_supported(VkPhysicalDevice physical_device, VkSurfaceKHR surface) {
        bool swap_chain_supported = false;
        auto [capabilities, formats, presentModes] = query_swap_chain_support(physical_device, surface);
        swap_chain_supported = !formats.is_empty() && !presentModes.is_empty();
        return swap_chain_supported;
    }

    bool VulkanDevice::QueueFamily::is_complete() const {
        return graphics_family != max_number && present_family != max_number;
    }

    VulkanDevice::VulkanDevice(VkInstance instance, VkSurfaceKHR surface) {
        uint32_t device_count = 0;
        vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
        if (device_count == 0) {
            startup_status = VK_ERROR_INITIALIZATION_FAILED;
            return;
        }
        VkPhysicalDevice physical_devices[device_count];
        vkEnumeratePhysicalDevices(instance, &device_count, physical_devices);

        graphics_device_ = physical_devices[0];
        for (int i = 0; i < device_count; i++) {
            VkPhysicalDevice physical_device = physical_devices[i];
            if (isDeviceSuitable(physical_device)) {
                graphics_device_ = physical_device;
                break;
            }
        }

        graphics_queue_family_ = find_queue_family(graphics_device_, surface);
        VkDeviceQueueCreateInfo single_queue{};
        VkDeviceQueueCreateInfo seperate_queues[2];
        VkDeviceQueueCreateInfo* queues;
        uint32_t queues_created = 0;
        constexpr float priority = 1.0f;
        if (graphics_queue_family_.graphics_family == graphics_queue_family_.present_family) {
            queues = &single_queue;
            single_queue.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            single_queue.queueFamilyIndex = graphics_queue_family_.graphics_family;
            single_queue.queueCount = 1;
            single_queue.pQueuePriorities = &priority;
            queues_created = 1;
        } else {
            queues = seperate_queues;
            for (int i = 0; i < 2; i++) {
                VkDeviceQueueCreateInfo queue_info = seperate_queues[i];
                single_queue.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                single_queue.queueFamilyIndex = i % 2 == 0 ? graphics_queue_family_.graphics_family : graphics_queue_family_.present_family;
                single_queue.queueCount = 1;
                single_queue.pQueuePriorities = &priority;
            }
            queues_created = 2;
        }

        VkPhysicalDeviceFeatures device_features{};
        vkGetPhysicalDeviceFeatures(graphics_device_, &device_features);

        VkDeviceCreateInfo device_create_info{};
        device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_create_info.queueCreateInfoCount = queues_created;
        device_create_info.pQueueCreateInfos = queues;
        device_create_info.pEnabledFeatures = &device_features;

        if (check_device_support(graphics_device_) && is_swap_chain_supported(graphics_device_, surface)) {
            device_create_info.enabledExtensionCount = NEEDED_EXTENSION_COUNT;
            device_create_info.ppEnabledExtensionNames = NEEDED_EXTENSIONS;
        } else {
            throw std::runtime_error("Failed to find a suitable device");
            startup_status = VK_ERROR_INITIALIZATION_FAILED;
            device_create_info.enabledExtensionCount = 0;
            device_create_info.ppEnabledExtensionNames = nullptr;
        }

        if (auto res = vkCreateDevice(graphics_device_, &device_create_info, nullptr, &logical_device_); res != VK_SUCCESS) {
            startup_status = res;
            return;
        }
        vkGetDeviceQueue(logical_device_, graphics_queue_family_.graphics_family, 0, &graphics_queue_);
        vkGetDeviceQueue(logical_device_, graphics_queue_family_.present_family, 0, &present_queue_);
    }

    VulkanDevice::~VulkanDevice() {
        if (startup_status != VK_SUCCESS) {
            return;
        }
        vkDestroyDevice(logical_device_, nullptr);
    }

    VkDevice VulkanDevice::get_logical_device() const {
        return logical_device_;
    }

    VkPhysicalDevice VulkanDevice::get_physical_device() const {
        return graphics_device_;
    }

    VkQueue VulkanDevice::get_graphics_queue() const {
        return graphics_queue_;
    }

    VkQueue VulkanDevice::get_present_queue() const {
        return present_queue_;
    }

    VulkanDevice::QueueFamily VulkanDevice::get_queue_family() const {
        return graphics_queue_family_;
    }

    bool VulkanDevice::was_startup_successful() const {
        return startup_status == VK_SUCCESS;
    }

    VkResult VulkanDevice::get_startup_status() const {
        return startup_status;
    }

    VulkanDevice::operator VkDevice() const {
        return logical_device_;
    }
}
