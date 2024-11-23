#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H

#include <vulkan/vulkan.hpp>

void transition_image(vk::CommandBuffer buffer, vk::Image image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);

vk::SemaphoreSubmitInfo submit_semaphore(vk::PipelineStageFlags2 flags, const vk::Semaphore& semaphore);
vk::SubmitInfo2 final_submit_info(const vk::CommandBufferSubmitInfo* cmd_buffer_submit,
    const vk::SemaphoreSubmitInfo* signal_semaphore,
    const vk::SemaphoreSubmitInfo* wait_semaphore);

#endif
