#ifndef SCREEN_H
#define SCREEN_H

#include <SDL_video.h>

namespace engine {
    class VulkanInstance {
        VkResult startup_status;
        SDL_Window *window_;
        VkInstance instance_;
        VkSurfaceKHR window_surface_{};

    public:
        VulkanInstance(uint32_t width, uint32_t height);
        ~VulkanInstance();

        [[nodiscard]] SDL_Window *get_raw_window() const;
        [[nodiscard]] VkInstance get_vk_instance() const;
        [[nodiscard]] VkSurfaceKHR get_surface_surface() const;

        [[nodiscard]] static bool hit_quit_key();
    };
}


#endif
