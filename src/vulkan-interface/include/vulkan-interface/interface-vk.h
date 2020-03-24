#ifndef VULKAN_INTERFACE_VK_H
#define VULKAN_INTERFACE_VK_H

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include "vulkan-interface/debug.h"
#include "vulkan-interface/device.h"
#include "vulkan-interface/extension.h"
#include "vulkan-interface/init.h"
#include "vulkan-interface/swapchain.h"
#include "language/optional.h"
#include "language/raw_vector.h"

struct VulkanState {
    GLFWwindow *window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debug_messenger;

    VkSurfaceKHR surface;

    struct InterfacePhysicalDevice physical_device;
    VkDevice logical_device;

    VkQueue graphics_queue;
    VkQueue presentation_queue;

    VkSwapchainKHR swapchain;
    VkFormat swapchain_format;
    VkExtent2D swapchain_extent;

    struct RawVector swapchain_images;
};

struct VulkanState vulkan_state_create(); 
void vulkan_state_destroy(struct VulkanState *state);


#endif
