#include <language/raw_vector.h>
#include <vulkan/vulkan.h>

struct RawVector create_command_buffers(
    VkDevice device, 
    VkCommandPool pool, 
    VkRenderPass renderpass, 
    VkPipeline pipeline,
    struct RawVector *rvec_VkFramebuffer, 
    VkExtent2D extent, 
    VkBuffer vertex_buffer);
VkCommandPool create_command_pool(VkDevice device, uint32_t qf_idx);