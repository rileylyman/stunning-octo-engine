#include <vulkan/vulkan.h>

VkPipeline create_graphics_pipeline(VkDevice device, VkExtent2D sc_extent, VkPipelineLayout *layout); 
VkShaderModule create_shader_module(VkDevice device, uint8_t *bytecode_buffer, size_t buffer_size); 