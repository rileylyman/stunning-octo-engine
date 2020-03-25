#include "vulkan-interface/interface-vk.h"
#include "vulkan-interface/pipeline.h"


//
// Initializes all Vulkan state
//
struct VulkanState vulkan_state_create() {

    GLFWwindow *window = init_window();
    VkInstance instance = init_vulkan();
#ifndef NDEBUG
    VkDebugUtilsMessengerEXT debug_messenger = init_vulkan_debug_messenger(instance);
#endif
    VkSurfaceKHR surface = create_surface(instance, window);
    struct InterfacePhysicalDevice physical_device = pick_physical_device(instance, surface);
    VkDevice logical_device = create_logical_device(&physical_device);

    VkQueue graphics_queue, presentation_queue; 
    vkGetDeviceQueue(logical_device, optional_index_get_value(&physical_device.graphics_family_index), 0, &graphics_queue);
    vkGetDeviceQueue(logical_device, optional_index_get_value(&physical_device.presentation_family_index), 0, &presentation_queue);

    VkFormat swapchain_format;
    VkExtent2D swapchain_extent;

    VkSwapchainKHR swapchain = create_swapchain(
        logical_device, 
        physical_device.physical_device,
        surface,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        optional_index_get_value(&physical_device.graphics_family_index),
        optional_index_get_value(&physical_device.presentation_family_index),
        &swapchain_format,
        &swapchain_extent
        );

    uint32_t image_count;
    vkGetSwapchainImagesKHR(logical_device, swapchain, &image_count, NULL);
    VkImage images[image_count];
    vkGetSwapchainImagesKHR(logical_device, swapchain, &image_count, images);
    struct RawVector swapchain_images_VkImage = raw_vector_create(sizeof(VkImage), image_count);
    raw_vector_extend_back(&swapchain_images_VkImage, images, image_count);

    struct RawVector swapchain_image_views_VkImageView = create_swapchain_image_views(
        logical_device, swapchain_images_VkImage, swapchain_format);

    VkPipelineLayout pipeline_layout;
    VkPipeline pipeline = create_graphics_pipeline(logical_device, swapchain_extent, &pipeline_layout);

    return (struct VulkanState) {
        .window = window,
        .instance = instance,
#ifndef NDEBUG
        .debug_messenger = debug_messenger,
#endif
        .physical_device = physical_device,
        .logical_device = logical_device,

        .graphics_queue = graphics_queue,
        .presentation_queue = presentation_queue,

        .surface = surface,

        .swapchain = swapchain,
        .swapchain_format = swapchain_format,
        .swapchain_extent = swapchain_extent,
        .swapchain_images_VkImage = swapchain_images_VkImage,
        .swapchain_image_views_VkImageView = swapchain_image_views_VkImageView,
        
        .pipeline = pipeline,
        .pipeline_layout = pipeline_layout, 
    };
} 

//
// Cleans up
//
void vulkan_state_destroy(struct VulkanState *state) {

    vkDestroyPipelineLayout(state->logical_device, state->pipeline_layout, NULL);
    vkDestroyPipeline(state->logical_device, state->pipeline, NULL);
    for (int i = 0; i < raw_vector_size(&state->swapchain_image_views_VkImageView); i++) {
        vkDestroyImageView(
            state->logical_device, 
            *(VkImageView *)raw_vector_get_ptr(&state->swapchain_image_views_VkImageView, i), 
            NULL);
    }
    raw_vector_destroy(&state->swapchain_image_views_VkImageView);
    raw_vector_destroy(&state->swapchain_images_VkImage);
    vkDestroySwapchainKHR(state->logical_device, state->swapchain, NULL);
    vkDestroySurfaceKHR(state->instance, state->surface, NULL);
    vkDestroyDevice(state->logical_device, NULL);
#ifndef NDEBUG
    DestroyDebugUtilsMessengerEXT(state->instance, state->debug_messenger, NULL);
#endif
    vkDestroyInstance(state->instance, NULL);
    glfwDestroyWindow(state->window);
    glfwTerminate();

}