//
// Created by LyftDriver on 11/25/2024.
//

#include "GraphicsPipeline.h"

namespace engine {

    VkRenderPass GraphicsPipeline::create_render_pass(VkDevice device, VkFormat swap_chain_format) {
        VkAttachmentDescription color_attachment{};
        color_attachment.format = swap_chain_format;
        color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference color_attachment_ref{};
        color_attachment_ref.attachment = 0;
        color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription sub_pass_description{};
        sub_pass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        sub_pass_description.colorAttachmentCount = 1;
        sub_pass_description.pColorAttachments = &color_attachment_ref;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo render_pass_create_info{};
        render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_create_info.attachmentCount = 1;
        render_pass_create_info.pAttachments = &color_attachment;
        render_pass_create_info.subpassCount = 1;
        render_pass_create_info.pSubpasses = &sub_pass_description;
        render_pass_create_info.dependencyCount = 1;
        render_pass_create_info.pDependencies = &dependency;

        VkRenderPass vk_render_pass;
        VkResult result = vkCreateRenderPass(device, &render_pass_create_info, nullptr, &vk_render_pass);
        return result == VK_SUCCESS ? vk_render_pass : VK_NULL_HANDLE;
    }

    VkPipeline GraphicsPipeline::create_graphics_pipeline(VkDevice device, SwapChain& swap_chain, VkPipelineLayout& pipeline_layout, VkRenderPass render_pass,
        VkPipelineShaderStageCreateInfo vertex_info, VkPipelineShaderStageCreateInfo fragment_info) {
        VkPipelineDynamicStateCreateInfo dynamic_state_create_info{};
        dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state_create_info.dynamicStateCount = 2;
        dynamic_state_create_info.pDynamicStates = dynamic_states;

        VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info{};
        vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_state_create_info.vertexBindingDescriptionCount = 0;
        vertex_input_state_create_info.pVertexBindingDescriptions = nullptr;
        vertex_input_state_create_info.vertexAttributeDescriptionCount = 0;
        vertex_input_state_create_info.pVertexAttributeDescriptions = nullptr;

        VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info{};
        input_assembly_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly_create_info.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swap_chain.get_swap_chain_extent().width);
        viewport.height = static_cast<float>(swap_chain.get_swap_chain_extent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swap_chain.get_swap_chain_extent();

        VkPipelineViewportStateCreateInfo viewport_state_create_info{};
        viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state_create_info.viewportCount = 1;
        viewport_state_create_info.pViewports = &viewport;
        viewport_state_create_info.scissorCount = 1;
        viewport_state_create_info.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterization_state_create_info{};
        rasterization_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterization_state_create_info.depthClampEnable = VK_FALSE;
        rasterization_state_create_info.rasterizerDiscardEnable = VK_FALSE;
        rasterization_state_create_info.polygonMode = VK_POLYGON_MODE_FILL;
        rasterization_state_create_info.lineWidth = 1.0f;
        rasterization_state_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterization_state_create_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterization_state_create_info.depthBiasEnable = VK_FALSE;
        rasterization_state_create_info.depthBiasConstantFactor = 0.0f;
        rasterization_state_create_info.depthBiasSlopeFactor = 0.0f;
        rasterization_state_create_info.depthBiasClamp = 0.0f;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f;
        multisampling.pSampleMask = nullptr;
        multisampling.alphaToCoverageEnable = VK_FALSE;
        multisampling.alphaToOneEnable = VK_FALSE;

        VkPipelineColorBlendAttachmentState color_blend_attachment{};
        color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        color_blend_attachment.blendEnable = VK_FALSE;
        color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
        color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &color_blend_attachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        VkPipelineLayoutCreateInfo pipeline_layout_create_info{};
        pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_create_info.setLayoutCount = 0;
        pipeline_layout_create_info.pSetLayouts = nullptr;
        pipeline_layout_create_info.pushConstantRangeCount = 0;
        pipeline_layout_create_info.pPushConstantRanges = nullptr;

        VkResult layout_create_result = vkCreatePipelineLayout(device, &pipeline_layout_create_info, nullptr, &pipeline_layout);
        if (layout_create_result != VK_SUCCESS) {
            return nullptr;
        }
        VkPipelineShaderStageCreateInfo stages[] = {vertex_info, fragment_info};
        VkGraphicsPipelineCreateInfo graphics_pipeline_create_info{};
        graphics_pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        graphics_pipeline_create_info.stageCount = 2;
        graphics_pipeline_create_info.pStages = stages;
        graphics_pipeline_create_info.pVertexInputState = &vertex_input_state_create_info;
        graphics_pipeline_create_info.pInputAssemblyState = &input_assembly_create_info;
        graphics_pipeline_create_info.pViewportState = &viewport_state_create_info;
        graphics_pipeline_create_info.pRasterizationState = &rasterization_state_create_info;
        graphics_pipeline_create_info.pMultisampleState = &multisampling;
        graphics_pipeline_create_info.pDepthStencilState = nullptr;
        graphics_pipeline_create_info.pColorBlendState = &colorBlending;
        graphics_pipeline_create_info.pDynamicState = &dynamic_state_create_info;

        graphics_pipeline_create_info.layout = pipeline_layout;
        graphics_pipeline_create_info.renderPass = render_pass;
        graphics_pipeline_create_info.subpass = 0;

        graphics_pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
        graphics_pipeline_create_info.basePipelineIndex = -1; // Optional

        VkPipeline graphics_pipeline;
        VkResult result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, nullptr, &graphics_pipeline);
        if (result != VK_SUCCESS) {
            return nullptr;
        }
        return graphics_pipeline;
    }

    VkSemaphore GraphicsPipeline::create_semaphore(VkDevice device) {
        VkSemaphoreCreateInfo semaphore_create_info{};
        semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        VkSemaphore semaphore;
        VkResult result = vkCreateSemaphore(device, &semaphore_create_info, nullptr, &semaphore);
        if (result != VK_SUCCESS) {
            return nullptr;
        }
        return semaphore;
    }

    VkFence GraphicsPipeline::create_fence(VkDevice device) {
        VkFenceCreateInfo fence_create_info{};
        fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        VkFence fence;
        if (vkCreateFence(device, &fence_create_info, nullptr, &fence) != VK_SUCCESS) {
            return nullptr;
        }
        return fence;
    }

    GraphicsPipeline::GraphicsPipeline(VulkanDevice& device_wrapper, SwapChain& swap_chain, VulkanDevice::QueueFamily queue_family) : device_wrapper_(device_wrapper), vertex_shader_("../shaders/triangle.vert.spv", device_wrapper),
        fragment_shader_("../shaders/triangle.frag.spv", device_wrapper),
        render_pass_(create_render_pass(device_wrapper, swap_chain.get_format())),
        graphics_pipeline_(create_graphics_pipeline(device_wrapper, swap_chain, pipeline_layout_, render_pass_,
            vertex_shader_.get_pipeline_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT),
            fragment_shader_.get_pipeline_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT))),
        queue_family_(queue_family),
        swap_chain_(swap_chain),
        image_available_semaphore_(create_semaphore(device_wrapper)),
        render_finished_semaphore_(create_semaphore(device_wrapper)),
        in_flight_fence_(create_fence(device_wrapper)),
        framebuffer_container_(device_wrapper, swap_chain.get_image_views(), render_pass_, swap_chain.get_swap_chain_extent()),
        command_container_(device_wrapper, render_pass_, graphics_pipeline_, queue_family, framebuffer_container_) {

    }

    GraphicsPipeline::~GraphicsPipeline() {
        vkDestroyFence(device_wrapper_, in_flight_fence_, nullptr);
        vkDestroySemaphore(device_wrapper_, render_finished_semaphore_, nullptr);
        vkDestroySemaphore(device_wrapper_, image_available_semaphore_, nullptr);
        vkDestroyPipeline(device_wrapper_, graphics_pipeline_, nullptr);
        vkDestroyPipelineLayout(device_wrapper_, pipeline_layout_, nullptr);
        vkDestroyRenderPass(device_wrapper_, render_pass_, nullptr);
    }

    VkPipeline GraphicsPipeline::get_graphics_pipeline() const {
        return graphics_pipeline_;
    }

    VkPipelineLayout GraphicsPipeline::get_pipeline_layout() const {
        return pipeline_layout_;
    }

    VkRenderPass GraphicsPipeline::get_render_pass() const {
        return render_pass_;
    }

    void GraphicsPipeline::draw() const {
        vkWaitForFences(device_wrapper_, 1, &in_flight_fence_, VK_TRUE, UINT64_MAX);
        vkResetFences(device_wrapper_, 1, &in_flight_fence_);

        uint32_t image_index;
        vkAcquireNextImageKHR(device_wrapper_, swap_chain_.get_swap_chain(), UINT64_MAX, image_available_semaphore_, VK_NULL_HANDLE, &image_index);
        command_container_.reset_command_buffer(0);
        if (command_container_.record_command_buffer(swap_chain_, image_index) != VK_SUCCESS) {
            return;
        }
        VkSubmitInfo submit_info{};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        constexpr VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = &image_available_semaphore_;
        submit_info.pWaitDstStageMask = wait_stages;

        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = &render_finished_semaphore_;

        if (vkQueueSubmit(device_wrapper_.get_graphics_queue(), 1, &submit_info, in_flight_fence_) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit graphics command buffer submission");
        }

        VkPresentInfoKHR present_info{};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = &render_finished_semaphore_;
        present_info.swapchainCount = 1;
        present_info.pSwapchains = swap_chain_.get_swap_chain_ref();
        present_info.pImageIndices = &image_index;
        present_info.pResults = nullptr;

        vkQueuePresentKHR(device_wrapper_.get_present_queue(), &present_info);

        vkDeviceWaitIdle(device_wrapper_);
    }
}
