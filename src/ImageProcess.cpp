#include "ImageProcess.h"

void transition_image(const vk::CommandBuffer buffer, const vk::Image image, const vk::ImageLayout oldLayout, const vk::ImageLayout newLayout) {
    const vk::ImageAspectFlags aspectFlags = newLayout == vk::ImageLayout::eDepthAttachmentOptimal
                                                 ? vk::ImageAspectFlagBits::eDepth
                                                 : vk::ImageAspectFlagBits::eColor;
    const vk::ImageSubresourceRange subresourceRange(aspectFlags, 0, VK_REMAINING_MIP_LEVELS, 0, VK_REMAINING_ARRAY_LAYERS);
    vk::ImageMemoryBarrier2 image_barrier{
        vk::PipelineStageFlagBits2::eAllCommands,
        vk::AccessFlagBits2::eMemoryWrite,
        vk::PipelineStageFlagBits2::eAllCommands,
        vk::AccessFlagBits2::eMemoryWrite | vk::AccessFlagBits2::eMemoryRead,
        oldLayout, newLayout, {}, {},
        image, subresourceRange
    };
    const vk::DependencyInfo dependency_info{{}, {}, {}, {}, {}, 1, &image_barrier};
    buffer.pipelineBarrier2(dependency_info);
}

vk::SemaphoreSubmitInfo submit_semaphore(vk::PipelineStageFlags2 flags, const vk::Semaphore &semaphore) {
    return vk::SemaphoreSubmitInfo{semaphore, 1, flags, 0};
}

vk::SubmitInfo2 final_submit_info(const vk::CommandBufferSubmitInfo* cmd_buffer_submit,
                                  const vk::SemaphoreSubmitInfo* signal_semaphore,
                                  const vk::SemaphoreSubmitInfo* wait_semaphore) {
    return vk::SubmitInfo2{{},
        static_cast<unsigned>(wait_semaphore == nullptr ? 0 : 1),
    wait_semaphore,
    1,
        cmd_buffer_submit,
    static_cast<unsigned>(signal_semaphore == nullptr ? 0 : 1),
    signal_semaphore};
}
