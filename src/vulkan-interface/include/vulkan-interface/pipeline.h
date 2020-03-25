#include <vulkan/vulkan.h>

VkPipeline create_graphics_pipeline(VkDevice device, VkExtent2D sc_extent, VkRenderPass renderpass, VkPipelineLayout *layout);
VkShaderModule create_shader_module(VkDevice device, uint8_t *bytecode_buffer, size_t buffer_size); 
VkRenderPass create_render_pass(VkDevice device, VkFormat image_format); 
struct RawVector create_framebuffers(VkDevice device, VkRenderPass renderpass, VkExtent2D extent, struct RawVector *rvec_VkImageView); 