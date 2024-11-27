#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
#include "DynArray.h"

namespace engine {

class FramebufferContainer {
    VkResult status_;
    VkDevice device_;
    DynArray<VkFramebuffer> frame_buffers_;
    VkRenderPass render_pass_;

public:
    FramebufferContainer(VkDevice device, const DynArray<VkImageView>& image_views, VkRenderPass render_pass,
                         VkExtent2D swap_chain_extent_2d);
    ~FramebufferContainer();

    VkFramebuffer& operator[](uint32_t index);
};

}

#endif
