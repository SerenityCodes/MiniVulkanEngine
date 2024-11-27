#include "VulkanInstance.h"

#include <iostream>
#include <SDL.h>
#include <SDL_vulkan.h>

#include "VulkanEngine.h"

namespace engine {
    const char* validation_layers[] = {"VK_LAYER_KHRONOS_validation"};
    constexpr int layer_count = 1;

    bool check_validation_layer_support(const char** validation_layers, const uint32_t wanted_layer_count) {
        uint32_t layer_count = 0;
        vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

        VkLayerProperties layer_names[layer_count];
        vkEnumerateInstanceLayerProperties(&layer_count, layer_names);
        for (uint32_t i = 0; i < wanted_layer_count; i++) {
            bool layerFound = false;

            for (const auto& layerProperties : layer_names) {
                if (strcmp(validation_layers[i], layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }
        return true;
    }

    VkInstanceCreateInfo create_instance_info(VkApplicationInfo& application_info, const char** extensions, uint32_t extension_count) {
        application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        application_info.apiVersion = VK_MAKE_VERSION(1, 2, 0);
        application_info.engineVersion = VK_MAKE_VERSION(0, 0, 1);
        application_info.pApplicationName = "Test Game";
        application_info.pEngineName = "Vulkan Engine";
        application_info.pNext = nullptr;
        application_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);

        VkInstanceCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.pApplicationInfo = &application_info;
        info.enabledExtensionCount = extension_count;
        info.ppEnabledExtensionNames = extensions;

#ifdef NDEBUG
        const bool enable_validationLayers = false;
#else
        const bool enable_validationLayers = true;
#endif
        if (enable_validationLayers && check_validation_layer_support(validation_layers, layer_count)) {
            info.enabledLayerCount = layer_count;
            info.ppEnabledLayerNames = validation_layers;
        } else {
            info.enabledLayerCount = 0;
            info.ppEnabledLayerNames = nullptr;
        }
        return info;
    }

    VulkanInstance::VulkanInstance(const uint32_t width, const uint32_t height) : startup_status(VK_SUCCESS) {
        SDL_Init(SDL_INIT_VIDEO);
        SDL_Vulkan_LoadLibrary(nullptr);
        window_ = SDL_CreateWindow("Test Vulkan Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, static_cast<int>(width), static_cast<int>(height), SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
        uint32_t extension_count = 0;
        SDL_Vulkan_GetInstanceExtensions(window_, &extension_count, nullptr);
        const char* extensions[extension_count];
        SDL_Vulkan_GetInstanceExtensions(window_, &extension_count, extensions);
        VkApplicationInfo application_info{};
        VkInstanceCreateInfo instance_create_info = create_instance_info(application_info, extensions, extension_count);
        if (const auto res = vkCreateInstance(&instance_create_info, nullptr, &instance_); res != VK_SUCCESS) {
            startup_status = res;
            return;
        }
        SDL_Vulkan_CreateSurface(window_, instance_, &window_surface_);
    }

    VulkanInstance::~VulkanInstance() {
        vkDestroySurfaceKHR(instance_, window_surface_, nullptr);
        vkDestroyInstance(instance_, nullptr);
        SDL_DestroyWindow(window_);
        SDL_Vulkan_UnloadLibrary();
        SDL_Quit();
    }

    SDL_Window *VulkanInstance::get_raw_window() const {
        return window_;
    }

    VkInstance VulkanInstance::get_vk_instance() const {
        return instance_;
    }

    VkSurfaceKHR VulkanInstance::get_surface_surface() const {
        return window_surface_;
    }

    bool VulkanInstance::hit_quit_key() {
        SDL_Event window_event;
        while (SDL_PollEvent(&window_event)) {
            if (window_event.type == SDL_QUIT) {
                return true;
            }
        }
        return false;
    }

}
