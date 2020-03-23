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
    //
    // Must create VkSurfaceKHR __before__ the physical device selection occurs.
    // The surface influences device selection.
    //
    VkSurfaceKHR surface = create_surface(instance, window);
    struct InterfacePhysicalDevice physical_device = pick_physical_device(instance, surface);
    VkDevice logical_device = create_logical_device(&physical_device);

    VkQueue graphics_queue; 
    vkGetDeviceQueue(logical_device, optional_index_get_value(&physical_device.graphics_family_index), 0, &graphics_queue);

    return (struct VulkanState) {
        .window = window,
        .instance = instance,
#ifndef NDEBUG
        .debug_messenger = debug_messenger,
#endif
        .physical_device = physical_device,
        .logical_device = logical_device,
        .graphics_queue = graphics_queue,
        .surface = surface,
    };
} 

//
// Cleans up
//
void vulkan_state_destroy(struct VulkanState *state) {

    vkDestroySurfaceKHR(state->instance, state->surface, NULL);
    vkDestroyDevice(state->logical_device, NULL);
#ifndef NDEBUG
    DestroyDebugUtilsMessengerEXT(state->instance, state->debug_messenger, NULL);
#endif
    vkDestroyInstance(state->instance, NULL);
    glfwDestroyWindow(state->window);
    glfwTerminate();

}