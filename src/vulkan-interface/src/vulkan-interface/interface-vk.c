#include "vulkan-interface/interface-vk.h"

//
// Initializes all Vulkan state
//
struct VulkanState vulkan_state_create() {

    GLFWwindow *window = init_window();
    VkInstance instance = init_vulkan();
#ifndef NDEBUG
    VkDebugUtilsMessengerEXT debug_messenger = init_vulkan_debug_messenger(instance);
#endif
    VkPhysicalDevice physical_device = pick_physical_device(instance);

    return (struct VulkanState) {
        .window = window,
        .instance = instance,
        .debug_messenger = debug_messenger,
        .physical_device = physical_device,
    };
} 

//
// Cleans up
//
void vulkan_state_destroy(struct VulkanState *state) {

#ifndef NDEBUG
    DestroyDebugUtilsMessengerEXT(state->instance, state->debug_messenger, NULL);
#endif
    vkDestroyInstance(state->instance, NULL);
    glfwDestroyWindow(state->window);
    glfwTerminate();

}