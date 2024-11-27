#include "SwapChain.h"

#include <iostream>
#include <SDL_vulkan.h>

#include <vulkan/vk_enum_string_helper.h>

#include "VulkanCommandContainer.h"
#include "VulkanDevice.h"

namespace engine {
    SwapChainSupportDetails query_swap_chain_support(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
        uint32_t format_count = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr);
        if (format_count != 0) {
            details.formats = DynArray<VkSurfaceFormatKHR>(format_count);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, details.formats.data());
        }
        uint32_t present_mode_count = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, nullptr);
        if (present_mode_count != 0) {
            details.presentModes = DynArray<VkPresentModeKHR>(present_mode_count);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, details.presentModes.data());
        }
        return details;
    }

    VkSurfaceFormatKHR choose_swap_surface_format(const DynArray<VkSurfaceFormatKHR>& available_formats) {
        for (auto it = available_formats.cbegin(); it != available_formats.cend(); ++it) {
            const auto& available_format = *it;
            if (available_format.format == VK_FORMAT_B8G8R8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return available_format;
            }
        }
        return available_formats[0];
    }

    VkPresentModeKHR choose_present_mode(const DynArray<VkPresentModeKHR>& available_present_modes) {
        for (auto it = available_present_modes.cbegin(); it != available_present_modes.cend(); ++it) {
            const auto& available_present_mode = *it;
            if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
                return available_present_mode;
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D choose_extent(const VkSurfaceCapabilitiesKHR& capabilities, SDL_Window* window) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        }
        int width, height;
        SDL_Vulkan_GetDrawableSize(window, &width, &height);
        VkExtent2D actual_extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
        actual_extent.width = glm::clamp(actual_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actual_extent.height = glm::clamp(actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
        return actual_extent;
    }

    SwapChain::SwapChain(const VulkanDevice& device, const VkSurfaceKHR& surface, SDL_Window* window) : device(device.get_logical_device()),
        physical_device_(device.get_physical_device()), queue_family_(device.get_queue_family()), window_surface(surface), window_(window) {
        startup_status = create_new_swap_chain();
    }

    SwapChain::~SwapChain() {
        for (const auto& image_view : image_views) {
            vkDestroyImageView(device, image_view, nullptr);
        }
        vkDestroySwapchainKHR(device, swap_chain, nullptr);
    }

    VkSwapchainKHR SwapChain::get_swap_chain() const {
        return swap_chain;
    }

    const VkSwapchainKHR* SwapChain::get_swap_chain_ref() const {
        return &swap_chain;
    }

    VkResult SwapChain::create_new_swap_chain() {
#if __cplusplus >= 202003L
        auto&[capabilities, formats, presentModes] = query_swap_chain_support(physical_device_, window_surface);
#else
        const SwapChainSupportDetails details = query_swap_chain_support(physical_device_, window_surface);
        auto& capabilities = details.capabilities;
        auto& formats = details.formats;
        auto& presentModes = details.presentModes;
#endif
        const VkSurfaceFormatKHR surface_format = choose_swap_surface_format(formats);
        VkPresentModeKHR present_mode = choose_present_mode(presentModes);
        const VkExtent2D extent = choose_extent(capabilities, window_);

        uint32_t image_count = capabilities.minImageCount + 1;
        if (capabilities.maxImageCount > 0 && image_count > capabilities.maxImageCount) {
            image_count = capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR swap_chain_create_info{};
        swap_chain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swap_chain_create_info.surface = window_surface;
        swap_chain_create_info.minImageCount = image_count;
        swap_chain_create_info.imageFormat = surface_format.format;
        swap_chain_create_info.imageColorSpace = surface_format.colorSpace;
        swap_chain_create_info.imageExtent = extent;
        swap_chain_create_info.imageArrayLayers = 1;
        swap_chain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        const uint32_t queue_family_indices[] = {queue_family_.graphics_family, queue_family_.present_family};
        if (queue_family_.graphics_family != queue_family_.present_family) {
            swap_chain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swap_chain_create_info.queueFamilyIndexCount = 2;
            swap_chain_create_info.pQueueFamilyIndices = queue_family_indices;
        } else {
            swap_chain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swap_chain_create_info.queueFamilyIndexCount = 0;
            swap_chain_create_info.pQueueFamilyIndices = nullptr;
        }
        swap_chain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swap_chain_create_info.presentMode = present_mode;
        swap_chain_create_info.clipped = VK_TRUE;
        swap_chain_create_info.oldSwapchain = VK_NULL_HANDLE;
        swap_chain_create_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;

        if (const auto res = vkCreateSwapchainKHR(device, &swap_chain_create_info, nullptr, &swap_chain); res != VK_SUCCESS) {
            return VK_ERROR_INITIALIZATION_FAILED;
        }

        uint32_t swap_chain_image_count = 0;
        vkGetSwapchainImagesKHR(device, swap_chain, &swap_chain_image_count, nullptr);
        images.resize(swap_chain_image_count);
        vkGetSwapchainImagesKHR(device, swap_chain, &swap_chain_image_count, images.data());

        image_format = surface_format.format;
        swap_chain_extent = extent;
        create_image_views();
        return VK_SUCCESS;
    }

    void SwapChain::create_image_views() {
        image_views.resize(images.get_size());
        for (uint32_t i = 0; i < image_views.get_size(); i++) {
            VkImageViewCreateInfo image_view_create_info{};
            image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            image_view_create_info.image = images[i];
            image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            image_view_create_info.format = image_format;
            image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            image_view_create_info.subresourceRange.baseMipLevel = 0;
            image_view_create_info.subresourceRange.levelCount = 1;
            image_view_create_info.subresourceRange.baseArrayLayer = 0;
            image_view_create_info.subresourceRange.layerCount = 1;

            if (vkCreateImageView(device, &image_view_create_info, nullptr, &image_views[i]) != VK_SUCCESS) {
                startup_status = VK_ERROR_INITIALIZATION_FAILED;
            }
        }
    }

    DynArray<VkImage>& SwapChain::get_images() {
        return images;
    }

    DynArray<VkImageView>& SwapChain::get_image_views() {
        return image_views;
    }

    VkFormat SwapChain::get_format() const {
        return image_format;
    }

    VkExtent2D SwapChain::get_swap_chain_extent() const {
        return swap_chain_extent;
    }

    SwapChain::operator VkSwapchainKHR() const {
        return swap_chain;
    }
}
