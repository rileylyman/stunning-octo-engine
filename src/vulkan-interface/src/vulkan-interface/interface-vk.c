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
    struct InterfacePhysicalDevice physical_device = pick_physical_device(instance);
    VkDevice logical_device = create_logical_device(&physical_device);

    VkQueue graphics_queue; 
    vkGetDeviceQueue(logical_device, optional_index_get_value(&physical_device.graphics_family_index), 0, &graphics_queue);

    return (struct VulkanState) {
        .window = window,
        .instance = instance,
        .debug_messenger = debug_messenger,
        .physical_device = physical_device,
        .logical_device = logical_device,
        .graphics_queue = graphics_queue,
    };
} 

//
// Cleans up
//
void vulkan_state_destroy(struct VulkanState *state) {

    vkDestroyDevice(state->logical_device, NULL);
#ifndef NDEBUG
    DestroyDebugUtilsMessengerEXT(state->instance, state->debug_messenger, NULL);
#endif
    vkDestroyInstance(state->instance, NULL);
    glfwDestroyWindow(state->window);
    glfwTerminate();

}