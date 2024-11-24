//> includes
#include "VulkanEngine.h"

#include <SDL.h>

#include <chrono>
#include <iostream>
#include <SDL_vulkan.h>
#include <thread>
#include <glm/detail/func_trigonometric.inl>

#include "ImageProcess.h"
#include "VkBootstrap.h"

void VulkanEngine::setup_vulkan() {
    vkb::InstanceBuilder instance_builder;
    auto inst_ret = instance_builder.set_app_name("MiniVulkanEngine")
        .request_validation_layers(true)
        .require_api_version(1, 3, 0)
        .build();
    instance_ = inst_ret.value().instance;

    VkSurfaceKHR c_surface;
    SDL_Vulkan_CreateSurface(_window, instance_, &c_surface);
    surface_ = c_surface;

    VkPhysicalDeviceVulkan13Features features{};
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    features.dynamicRendering = true;
    features.synchronization2 = true;
    VkPhysicalDeviceVulkan12Features feature_12{};
    feature_12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    feature_12.bufferDeviceAddress = true;
    feature_12.descriptorIndexing = true;
    vkb::PhysicalDeviceSelector device_selector(inst_ret.value(), c_surface);
    vkb::PhysicalDevice selected_device = device_selector
        .set_minimum_version(1, 3)
        .set_required_features_13(features)
        .set_required_features_12(feature_12)
        .select()
        .value();
    vkb::DeviceBuilder device_builder(selected_device);
    vkb::Device built_device = device_builder.build().value();
    device_ = built_device;
    physical_device_ = selected_device;

    graphics_queue_ = built_device.get_queue(vkb::QueueType::graphics).value();
    graphics_queue_family_index_ = built_device.get_queue_index(vkb::QueueType::graphics).value();
}

void VulkanEngine::setup_swap_chain(uint32_t width, uint32_t height) {
    vkb::SwapchainBuilder swap_chain_builder(physical_device_, device_, surface_);
    swap_chain_format_ = vk::Format::eR8G8B8A8Snorm;
    vkb::Swapchain built_swap_chain = swap_chain_builder
        .set_desired_format(vk::SurfaceFormatKHR(swap_chain_format_, vk::ColorSpaceKHR::eSrgbNonlinear))
        .set_desired_present_mode(static_cast<VkPresentModeKHR>(vk::PresentModeKHR::eMailbox))
        .set_desired_extent(width, height)
        .add_image_usage_flags(static_cast<VkImageUsageFlags>(vk::ImageUsageFlagBits::eTransferDst))
        .build().value();
    swap_chain_extent_ = built_swap_chain.extent;
    swap_chain_ = built_swap_chain.swapchain;
    // Implicit conversion to C++ binding using iterator copying
    auto chain_images = built_swap_chain.get_images().value();
    swap_chain_images_ = {chain_images.begin(), chain_images.end()};
    auto chain_image_views = built_swap_chain.get_image_views().value();
    swap_chain_image_views_ = {chain_image_views.begin(), chain_image_views.end()};
}

void VulkanEngine::setup_command_buffers() {
    vk::CommandPoolCreateInfo command_pool_create_info{vk::CommandPoolCreateFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer),
        graphics_queue_family_index_};
    for (auto &frame : frames_) {
        frame.command_pool = device_.createCommandPool(command_pool_create_info, nullptr);
        vk::CommandBufferAllocateInfo command_allocate_info(frame.command_pool, vk::CommandBufferLevel::ePrimary, 1);
        // Returns a vector. We're just allocating one command buffer for now
        frame.command_buffer = device_.allocateCommandBuffers(command_allocate_info)[0];
    }
}

void VulkanEngine::setup_sync_structures() {
    vk::FenceCreateInfo fence_info{vk::FenceCreateFlags(vk::FenceCreateFlagBits::eSignaled)};
    vk::SemaphoreCreateInfo semaphore_create_info{vk::SemaphoreCreateFlags()};
    for (auto& frame : frames_) {
        frame.render_fence = device_.createFence(fence_info);
        frame.render_semaphore = device_.createSemaphore(semaphore_create_info);
        frame.swap_chain_semaphore = device_.createSemaphore(semaphore_create_info);
    }
}

VulkanEngine::VulkanEngine() : deletion_queue() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_WindowFlags window_flags = SDL_WINDOW_VULKAN;

    _window = SDL_CreateWindow(
        "Vulkan Engine",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        static_cast<int>(window_extent_.width),
        static_cast<int>(window_extent_.height),
        window_flags);

    setup_vulkan();
    setup_swap_chain(window_extent_.width, window_extent_.height);
    setup_command_buffers();
    setup_sync_structures();

    // everything went fine
    _isInitialized = true;
}

VulkanEngine::~VulkanEngine() {
    if (!_isInitialized) {
        return;
    }
    vkDeviceWaitIdle(device_);
    // Clean up command pools and sync objects
    for (auto& [deletion_queue, command_pool, cmd_buffer, render_fence, render_semaphore, swap_chain_semaphore] : frames_) {
        device_.destroyCommandPool(command_pool);
        device_.destroyFence(render_fence);
        device_.destroySemaphore(render_semaphore);
        device_.destroySemaphore(swap_chain_semaphore);
        deletion_queue.flush();
    }

    vkDestroySwapchainKHR(device_, swap_chain_, nullptr);
    for (const auto& image_view : swap_chain_image_views_) {
        vkDestroyImageView(device_, image_view, nullptr);
    }
    vkDestroySurfaceKHR(instance_, surface_, nullptr);
    vkDestroyDevice(device_, nullptr);
    vkDestroyInstance(instance_, nullptr);
    SDL_DestroyWindow(_window);
}

void VulkanEngine::draw() {
    constexpr uint64_t one_second = 1000000000;
    auto&[deletion_queue, command_pool, command_buffer, render_fence, render_semaphore, swap_chain_semaphore] = current_frame();
    if (const vk::Result fence_wait_result = device_.waitForFences(1, &render_fence, true, one_second);
        fence_wait_result != vk::Result::eSuccess) {
        throw std::runtime_error("VulkanEngine failed to wait for a frame");
    }
    deletion_queue.flush();
    if (const vk::Result fence_reset_result = device_.resetFences(1, &render_fence);
        fence_reset_result != vk::Result::eSuccess) {
        throw std::runtime_error("VulkanEngine failed to reset fence");
    }

    const auto next_image_index = device_.acquireNextImageKHR(swap_chain_, UINT64_MAX, swap_chain_semaphore);
    if (next_image_index.result != vk::Result::eSuccess) {
        throw std::runtime_error("VulkanEngine failed to acquire swap chain image");
    }
    uint32_t swap_chain_image_index = next_image_index.value;

    const vk::CommandBuffer& main_buffer = command_buffer;
    main_buffer.reset();
    constexpr vk::CommandBufferBeginInfo begin_info{vk::CommandBufferUsageFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit), nullptr};
    main_buffer.begin(begin_info);

    transition_image(main_buffer, swap_chain_images_[swap_chain_image_index], vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);

    const float flash = glm::abs(glm::sin(static_cast<float>(frame_number_) / 120.f));
    const vk::ClearColorValue clear_color_value{0.0f, 0.0f, flash, 1.0f};
    constexpr vk::ImageSubresourceRange clear_range{vk::ImageAspectFlagBits::eColor, 0, VK_REMAINING_MIP_LEVELS, 0, VK_REMAINING_ARRAY_LAYERS};
    main_buffer.clearColorImage(swap_chain_images_[swap_chain_image_index], vk::ImageLayout::eGeneral, clear_color_value, clear_range);
    transition_image(main_buffer, swap_chain_images_[swap_chain_image_index], vk::ImageLayout::eGeneral, vk::ImageLayout::ePresentSrcKHR);

    main_buffer.end();

    const vk::CommandBufferSubmitInfo cmd_buffer_submit_info{main_buffer};
    const vk::SemaphoreSubmitInfo wait_info = submit_semaphore(vk::PipelineStageFlagBits2::eColorAttachmentOutput, swap_chain_semaphore);
    const vk::SemaphoreSubmitInfo signal_info = submit_semaphore(vk::PipelineStageFlagBits2::eAllGraphics, render_semaphore);
    const vk::SubmitInfo2 cmd_buffer_submit = final_submit_info(&cmd_buffer_submit_info, &signal_info, &wait_info);
    if (const auto result = graphics_queue_.submit2(1, &cmd_buffer_submit, render_fence);
        result != vk::Result::eSuccess) {
        throw std::runtime_error("Graphics queue submit failed");
    }
    if (const vk::PresentInfoKHR present_info{1,
        &render_semaphore,
        1,
        &swap_chain_,
        &swap_chain_image_index}; graphics_queue_.presentKHR(present_info) != vk::Result::eSuccess) {
        throw std::runtime_error("Graphics queue presentKHR failed");
    }

    frame_number_++;
}

void VulkanEngine::run() {
    SDL_Event e;
    bool bQuit = false;

    // main loop
    while (!bQuit) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            // close the window when user alt-f4s or clicks the X button
            if (e.type == SDL_QUIT)
                bQuit = true;

            if (e.type == SDL_WINDOWEVENT) {
                if (e.window.event == SDL_WINDOWEVENT_MINIMIZED) {
                    stop_rendering = true;
                }
                if (e.window.event == SDL_WINDOWEVENT_RESTORED) {
                    stop_rendering = false;
                }
            }
        }

        // do not draw if we are minimized
        if (stop_rendering) {
            // throttle the speed to avoid the endless spinning
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        draw();
    }
}