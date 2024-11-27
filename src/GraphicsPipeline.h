#ifndef GRAPHICSPIPELINE_H
#define GRAPHICSPIPELINE_H

#include "FramebufferContainer.h"
#include "ShaderModule.h"
#include "SwapChain.h"
#include "VulkanCommandContainer.h"

namespace engine {
    inline constexpr VkDynamicState dynamic_states[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    class GraphicsPipeline {
        VkResult result_ = VK_SUCCESS;
        VkResult render_pass_creation = VK_SUCCESS;
        VkResult graphics_pipeline_creation = VK_SUCCESS;

        VulkanDevice& device_wrapper_;
        ShaderModule vertex_shader_;
        ShaderModule fragment_shader_;
        VkRenderPass render_pass_;
        VkPipelineLayout pipeline_layout_{};
        VkPipeline graphics_pipeline_{};
        VulkanDevice::QueueFamily queue_family_;
        SwapChain& swap_chain_;
        VkSemaphore image_available_semaphore_;
        VkSemaphore render_finished_semaphore_;
        VkFence in_flight_fence_;
        FramebufferContainer framebuffer_container_;
        VulkanCommandContainer command_container_;

        static VkPipeline create_graphics_pipeline(VkDevice device, SwapChain& swap_chain, VkPipelineLayout& pipeline_layout, VkRenderPass render_pass, VkPipelineShaderStageCreateInfo vertex_info, VkPipelineShaderStageCreateInfo fragment_info);
        static VkSemaphore create_semaphore(VkDevice device);
        static VkFence create_fence(VkDevice device);
        static VkRenderPass create_render_pass(VkDevice device, VkFormat swap_chain_format);
    public:
        GraphicsPipeline(VulkanDevice& device_wrapper, SwapChain& swap_chain, VulkanDevice::QueueFamily queue_family_);
        ~GraphicsPipeline();

        [[nodiscard]] VkPipeline get_graphics_pipeline() const;
        [[nodiscard]] VkPipelineLayout get_pipeline_layout() const;
        [[nodiscard]] VkRenderPass get_render_pass() const;

        void draw() const;
    };

}

#endif
