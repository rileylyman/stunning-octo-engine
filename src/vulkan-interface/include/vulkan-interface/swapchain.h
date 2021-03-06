#include <vulkan/vulkan.h>
#include <stdbool.h>
#include <language/raw_vector.h>

bool is_swapchain_adequate(VkPhysicalDevice device, VkSurfaceKHR surface); 

VkSwapchainKHR create_swapchain(
    VkDevice logical_device,
    VkPhysicalDevice physical_device, 
    VkSurfaceKHR surface, 
    uint32_t window_width, 
    uint32_t window_height,
    uint32_t graphics_qfidx,
    uint32_t present_qfidx,
    VkFormat *fill_format,
    VkExtent2D *fill_extent); 

struct RawVector create_swapchain_image_views(VkDevice device, struct RawVector rvec_VkImage, VkFormat format);
