#ifndef RENDERPASS_H
#define RENDERPASS_H

namespace engine {

class RenderPass {
    VkRenderPass render_pass_;
public:
    RenderPass();
    ~RenderPass();
};

}

#endif
