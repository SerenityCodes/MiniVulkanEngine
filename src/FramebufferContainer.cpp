#include "FramebufferContainer.h"

namespace engine {
    FramebufferContainer::FramebufferContainer(VkDevice device, const DynArray<VkImageView>& image_views, VkRenderPass render_pass, const VkExtent2D swap_chain_extent_2d) :
        status_(VK_SUCCESS),
        device_(device),
        frame_buffers_(image_views.get_size()),
        render_pass_(render_pass) {

        for (size_t i = 0; i < image_views.get_size(); i++) {
            const VkImageView attachments[] = {
                image_views[i]
            };

            VkFramebufferCreateInfo framebuffer_create_info{};
            framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebuffer_create_info.renderPass = render_pass;
            framebuffer_create_info.attachmentCount = 1;
            framebuffer_create_info.pAttachments = attachments;
            framebuffer_create_info.width = swap_chain_extent_2d.width;
            framebuffer_create_info.height = swap_chain_extent_2d.height;
            framebuffer_create_info.layers = 1;

            if (vkCreateFramebuffer(device, &framebuffer_create_info, nullptr, &frame_buffers_[i]) != VK_SUCCESS) {
                status_ = VK_ERROR_INITIALIZATION_FAILED;
            }
        }
    }

    FramebufferContainer::~FramebufferContainer() {
        for (const auto frame_buffer : frame_buffers_) {
            vkDestroyFramebuffer(device_, frame_buffer, nullptr);
        }
    }

    VkFramebuffer& FramebufferContainer::operator[](const uint32_t index) {
        return frame_buffers_[index];
    }
}