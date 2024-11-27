#ifndef SHADERMODULE_H
#define SHADERMODULE_H
#include "DynArray.h"

namespace engine {

class ShaderModule {
    VkResult result_;
    VkDevice device_;
    VkShaderModule shader_module_;

public:
    ShaderModule(const char* file_name, VkDevice device_);
    ~ShaderModule();

    VkShaderModule& get_shader_module();
    [[nodiscard]] VkPipelineShaderStageCreateInfo get_pipeline_stage_create_info(VkShaderStageFlagBits stage) const;
};

} // engine

#endif //SHADERMODULE_H
