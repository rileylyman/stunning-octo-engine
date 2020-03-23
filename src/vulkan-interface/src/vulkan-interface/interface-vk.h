#ifndef VULKAN_INTERFACE_VK_H
#define VULKAN_INTERFACE_VK_H

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include "vulkan-interface/debug.h"
#include "vulkan-interface/device.h"
#include "vulkan-interface/extension.h"
#include "vulkan-interface/init.h"

struct VulkanState {
    GLFWwindow *window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debug_messenger;
    VkPhysicalDevice physical_device;
    VkDevice logical_device;
};

struct VulkanState vulkan_state_create(); 
void vulkan_state_destroy(struct VulkanState *state);


#endif
