#include "ShaderModule.h"

#include "VulkanEngine.h"

namespace engine {
    ShaderModule::ShaderModule(const char* file_name, const VkDevice device_) : device_(device_), shader_module_(nullptr) {
        const uint32_t file_size = VulkanEngine::get_file_size(file_name);
        char code[file_size];
        VulkanEngine::read_file(file_name, code, file_size);
        VkShaderModuleCreateInfo shader_module_create_info{};
        shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shader_module_create_info.codeSize = file_size;
        shader_module_create_info.pCode = reinterpret_cast<uint32_t*>(code);

        if (vkCreateShaderModule(device_, &shader_module_create_info, nullptr, &shader_module_) != VK_SUCCESS) {
            result_ = VK_ERROR_INITIALIZATION_FAILED;
            return;
        }
    }

    ShaderModule::~ShaderModule() {
        vkDestroyShaderModule(device_, shader_module_, nullptr);
    }

    VkShaderModule& ShaderModule::get_shader_module() {
        return shader_module_;
    }

    VkPipelineShaderStageCreateInfo ShaderModule::get_pipeline_stage_create_info(const VkShaderStageFlagBits stage) const {
        VkPipelineShaderStageCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        create_info.stage = stage;
        create_info.module = shader_module_;
        create_info.pName = "main";
        create_info.pSpecializationInfo = nullptr;
        return create_info;
    }
}
