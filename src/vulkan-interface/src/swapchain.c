#include "vulkan-interface/swapchain.h"
#include "language/math.h"
#include "log.h"
#include <stdlib.h>

//
// Returns true if the swapchain from this device to the given surface
// is adequate for use by our engine. 
//
bool is_swapchain_adequate(VkPhysicalDevice device, VkSurfaceKHR surface) {

    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, NULL);

    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, NULL);

    //
    // For now, adequate means that there is at least one present mode and one format
    // for the swapchain. 
    //

    return format_count > 0 && present_mode_count > 0;
}

//
// Creates the swapchain from this device to this surface.
//
VkSwapchainKHR create_swapchain(
    VkDevice logical_device,
    VkPhysicalDevice physical_device, 
    VkSurfaceKHR surface, 
    uint32_t window_width, 
    uint32_t window_height,
    uint32_t graphics_qfidx,
    uint32_t present_qfidx,
    VkFormat *fill_format,
    VkExtent2D *fill_extent) {

    //
    // Choose surface format
    //
    VkSurfaceFormatKHR format;
    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, NULL);
    VkSurfaceFormatKHR formats[format_count];
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, formats);

    format = formats[0];
    for (int i = 0; i < format_count; i++) {
        if (formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            format = formats[i];
            break;
        }
    }

    //
    // Choose surface presentation mode
    //
    VkPresentModeKHR present_mode;
    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, NULL);
    VkPresentModeKHR present_modes[present_mode_count];
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, present_modes);

    present_mode = VK_PRESENT_MODE_FIFO_KHR;
    for (int i = 0; i < present_mode_count; i++) {
        if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            present_mode = present_modes[i];
            break;
        }
    }

    //
    // Choose swapchain extent
    //
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &capabilities);
    if (capabilities.currentExtent.width == UINT32_MAX) {
        capabilities.currentExtent.width = MAX(capabilities.minImageExtent.width, MIN(capabilities.maxImageExtent.width, window_width));
        capabilities.currentExtent.height = MAX(capabilities.minImageExtent.height, MIN(capabilities.maxImageExtent.height, window_height));
    }

    //
    // Choose number of images in the swapchain
    //
    uint32_t image_count = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0) {
        image_count = MIN(image_count, capabilities.maxImageCount);
    } 

    //
    // Populate create info for the swapchain
    //
    VkSwapchainCreateInfoKHR create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = surface;
    create_info.minImageCount = image_count;
    create_info.imageFormat = format.format;
    create_info.imageColorSpace = format.colorSpace;
    create_info.imageExtent = capabilities.currentExtent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // Note: can be setup as a transfer to do post-processing

    if (graphics_qfidx != present_qfidx) {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = (uint32_t[]){graphics_qfidx, present_qfidx};
    } else {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    create_info.preTransform = capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE; 
    create_info.oldSwapchain = VK_NULL_HANDLE;

    //
    // Create the swapchain and return it
    //  
    VkSwapchainKHR swapchain;
    if (vkCreateSwapchainKHR(logical_device, &create_info, NULL, &swapchain) != VK_SUCCESS) {
        log_fatal("Failed to create swapchain!");
        exit(EXIT_FAILURE);
    }

    log_trace("Created swapchain!\n");
    *fill_format = create_info.imageFormat;
    *fill_extent = create_info.imageExtent;
    return swapchain;
}

//
// Create an image view for each image within rvec_VkImage
//
struct RawVector create_swapchain_image_views(VkDevice device, struct RawVector rvec_VkImage, VkFormat format) {

    struct RawVector image_views_VkImageView = raw_vector_create(sizeof(VkImageView), raw_vector_size(&rvec_VkImage));
    VkImageView blank_view;

    for (int i = 0; i < raw_vector_size(&rvec_VkImage); i++) {

        VkImageViewCreateInfo ci = {};
        ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        ci.image = *(VkImage *)raw_vector_get_ptr(&rvec_VkImage, i);
        ci.viewType = VK_IMAGE_VIEW_TYPE_2D;
        ci.format = format;
        ci.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        ci.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        ci.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        ci.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        ci.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        ci.subresourceRange.baseMipLevel = 0;
        ci.subresourceRange.levelCount = 1;
        ci.subresourceRange.baseArrayLayer = 0;
        ci.subresourceRange.layerCount = 1;

        //
        // For each image view create info we make, we push back 
        // a blank image view into our image views vector. This is
        // so that on the next line, when it comes time to call the vulkan
        // constructor for the image view, we can get a ptr reference
        // into the vector into which the constructor can copy the
        // new image view.
        //
        raw_vector_push_back(&image_views_VkImageView, &blank_view);
        if (vkCreateImageView(device, &ci, NULL, (VkImageView *)raw_vector_get_ptr(&image_views_VkImageView, i)) != VK_SUCCESS) {
            log_fatal("Failed to create image view %i\n", i);
            exit(EXIT_FAILURE);
        }
        log_trace("Created image view %i\n", i);
    }

    return image_views_VkImageView;
}
