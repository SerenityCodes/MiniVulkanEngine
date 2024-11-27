#ifndef SWAP_CHAIN_H
#define SWAP_CHAIN_H
#include <SDL_video.h>

#include "DynArray.h"
#include "VulkanDevice.h"

namespace engine {

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        DynArray<VkSurfaceFormatKHR> formats;
        DynArray<VkPresentModeKHR> presentModes;
    };

    SwapChainSupportDetails query_swap_chain_support(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);

    class SwapChain {
        VkResult startup_status;
        VkDevice device;
        VkPhysicalDevice physical_device_;
        VulkanDevice::QueueFamily queue_family_;
        VkSurfaceKHR window_surface;
        SDL_Window* window_;
        VkSwapchainKHR swap_chain;
        VkFormat image_format;
        VkExtent2D swap_chain_extent;
        DynArray<VkImage> images;
        DynArray<VkImageView> image_views;

    public:
        SwapChain(const VulkanDevice &device, const VkSurfaceKHR &surface, SDL_Window *window);

        ~SwapChain();

        [[nodiscard]] VkSwapchainKHR get_swap_chain() const;

        [[nodiscard]] const VkSwapchainKHR* get_swap_chain_ref() const;

        VkResult create_new_swap_chain();
        void create_image_views();
        [[nodiscard]] DynArray<VkImage>& get_images();
        [[nodiscard]] DynArray<VkImageView>& get_image_views();
        [[nodiscard]] VkFormat get_format() const;

        [[nodiscard]] VkExtent2D get_swap_chain_extent() const;

        operator VkSwapchainKHR() const;
    };
}


#endif
