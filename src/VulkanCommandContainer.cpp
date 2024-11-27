#include "VulkanCommandContainer.h"

#include "SwapChain.h"

namespace engine {
    VulkanCommandContainer::VulkanCommandContainer(VkDevice device, VkRenderPass render_pass, VkPipeline pipeline, const VulkanDevice::QueueFamily& queue_family, FramebufferContainer& container) :
        result_(VK_SUCCESS), device_(device), render_pass_(render_pass), graphics_pipeline_(pipeline), frame_buffers_(container) {
        VkCommandPoolCreateInfo command_pool_create_info{};
        command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        command_pool_create_info.queueFamilyIndex = queue_family.graphics_family;
        if (vkCreateCommandPool(device_, &command_pool_create_info, nullptr, &command_pool_) != VK_SUCCESS) {

        }
        VkCommandBufferAllocateInfo command_buffer_allocate_info{};
        command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        command_buffer_allocate_info.commandPool = command_pool_;
        command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        command_buffer_allocate_info.commandBufferCount = 1;
        if (vkAllocateCommandBuffers(device_, &command_buffer_allocate_info, &primary_command_buffer_)) {

        }
    }

    VulkanCommandContainer::~VulkanCommandContainer() {
        vkDestroyCommandPool(device_, command_pool_, nullptr);
    }

    VkResult VulkanCommandContainer::record_command_buffer(const SwapChain& swap_chain, uint32_t image_index) const {
        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = 0;
        begin_info.pInheritanceInfo = nullptr;

        if (const auto res = vkBeginCommandBuffer(primary_command_buffer_, &begin_info); res != VK_SUCCESS) {
            return res;
        }
        auto current_frame_buffer = frame_buffers_[image_index];
        VkRenderPassBeginInfo render_pass_info{};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_info.renderPass = render_pass_;
        render_pass_info.framebuffer = current_frame_buffer;
        render_pass_info.renderArea.offset = {0, 0};
        render_pass_info.renderArea.extent = swap_chain.get_swap_chain_extent();

        constexpr VkClearValue clear_color = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        render_pass_info.clearValueCount = 1;
        render_pass_info.pClearValues = &clear_color;

        vkCmdBeginRenderPass(primary_command_buffer_, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(primary_command_buffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swap_chain.get_swap_chain_extent().width);
        viewport.height = static_cast<float>(swap_chain.get_swap_chain_extent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(primary_command_buffer_, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swap_chain.get_swap_chain_extent();
        vkCmdSetScissor(primary_command_buffer_, 0, 1, &scissor);

        vkCmdDraw(primary_command_buffer_, 3, 1, 0, 0);
        vkCmdEndRenderPass(primary_command_buffer_);

        if (auto res = vkEndCommandBuffer(primary_command_buffer_); res != VK_SUCCESS) {
            return res;
        }
        return VK_SUCCESS;
    }

    void VulkanCommandContainer::reset_command_buffer(VkCommandBufferResetFlags reset_flags) const {
        vkResetCommandBuffer(primary_command_buffer_, reset_flags);
    }
}
