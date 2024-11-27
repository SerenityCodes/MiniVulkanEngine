# Add source to this project's executable.
add_executable (engine 
    main.cpp
    VulkanEngine.cpp
    VulkanEngine.h
    SwapChain.cpp
    SwapChain.h
    VulkanInstance.cpp
    VulkanInstance.h
    VulkanDevice.cpp
    VulkanDevice.h
    aliases.h
    DynArray.h
    ShaderModule.cpp
    ShaderModule.h
    GraphicsPipeline.cpp
    GraphicsPipeline.h
    RenderPass.cpp
    RenderPass.h
    FramebufferContainer.cpp
    FramebufferContainer.h
    VulkanCommandContainer.cpp
    VulkanCommandContainer.h
    CArrayRef.h
    StackAllocator.h
    StackAllocator.cpp
)

add_dependencies(engine Shaders)

add_compile_options(-Wall -Werror -std=c++20 -Wpedantic -Wconversion -Wcast-align -Wunused -Wshadow -Wold-style-cast -Wpointer-arith -Wcast-qual -Wno-missing-braces -Wfatal-errors)

set_property(TARGET engine PROPERTY CXX_STANDARD 20)
target_compile_definitions(engine PUBLIC GLM_FORCE_DEPTH_ZERO_TO_ONE)
target_include_directories(engine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(engine PUBLIC vma glm Vulkan::Vulkan fmt::fmt stb_image SDL2::SDL2 imgui fastgltf::fastgltf)

target_precompile_headers(engine PUBLIC <optional> <vector> <memory> <string> <vector> <unordered_map> <glm/mat4x4.hpp>  <glm/vec4.hpp> <vulkan/vulkan.h>)

add_custom_command(TARGET engine POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:engine> $<TARGET_FILE_DIR:engine>
  COMMAND_EXPAND_LISTS
  )