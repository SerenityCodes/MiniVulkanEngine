// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vulkan\vulkan.hpp>

#include "DeletionQueue.h"

struct FrameData {
	DeletionQueue deletion_queue;
	vk::CommandPool command_pool;
	vk::CommandBuffer command_buffer;
	vk::Fence render_fence;
	vk::Semaphore render_semaphore, swap_chain_semaphore;
};

constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;

class VulkanEngine {
	DeletionQueue deletion_queue;

	vk::Instance instance_;
	vk::PhysicalDevice physical_device_;
	vk::Device device_;
	vk::SurfaceKHR surface_;
	vk::Extent2D window_extent_{1600, 700};

	vk::Extent2D swap_chain_extent_;
	vk::SwapchainKHR swap_chain_;
	vk::Format swap_chain_format_;
	std::vector<vk::Image> swap_chain_images_;
	std::vector<vk::ImageView> swap_chain_image_views_;

	FrameData frames_[MAX_FRAMES_IN_FLIGHT];
	FrameData& current_frame() { return frames_[frame_number_ % (MAX_FRAMES_IN_FLIGHT)]; };
	vk::Queue graphics_queue_;
	uint32_t graphics_queue_family_index_;

	void setup_vulkan();

	void setup_swap_chain(uint32_t width, uint32_t height);
	void setup_command_buffers();
	void setup_sync_structures();
public:

	bool _isInitialized = false;
	int frame_number_ = 0;
	bool stop_rendering = false;

	struct SDL_Window* _window{ nullptr };

	VulkanEngine();
	~VulkanEngine();

	//draw loop
	void draw();

	//run main loop
	void run();
};
