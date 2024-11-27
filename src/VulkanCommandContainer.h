#ifndef VULKANCOMMANDCONTAINER_H
#define VULKANCOMMANDCONTAINER_H

#include "FramebufferContainer.h"
#include "SwapChain.h"
#include "VulkanDevice.h"

namespace engine {

class VulkanCommandContainer {
    VkResult result_;
    VkDevice device_;
    VkRenderPass render_pass_;
    VkCommandPool command_pool_{};
    VkCommandBuffer primary_command_buffer_{};
    VkPipeline graphics_pipeline_;
    FramebufferContainer& frame_buffers_;

public:
    VulkanCommandContainer(VkDevice device, VkRenderPass render_pass, VkPipeline pipeline, const VulkanDevice::QueueFamily& queue_family, FramebufferContainer& container);
    ~VulkanCommandContainer();

    [[nodiscard]] VkResult record_command_buffer(const SwapChain &swap_chain, uint32_t image_index) const;
    void reset_command_buffer(VkCommandBufferResetFlags reset_flags) const;
};

}

#endif
