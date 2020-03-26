#include "vulkan-interface/interface-vk.h"
#include "vulkan-interface/pipeline.h"

#define MAX_FRAMES_IN_FLIGHT 2

static bool glfw_window_resized = false;
static void framebuffer_resize_callback(GLFWwindow *window, int width, int height) {
    glfw_window_resized = true;
}

void main_loop(struct VulkanState *state) {
    //
    // Create synchronization primitives
    //
    VkSemaphore imageAvailableSemaphores[MAX_FRAMES_IN_FLIGHT];
    VkSemaphore renderFinishedSemaphores[MAX_FRAMES_IN_FLIGHT];
    VkFence     frameFences             [MAX_FRAMES_IN_FLIGHT];
    VkFence     imageFences             [raw_vector_size(&state->swapchain_images_VkImage)];
    memset(imageFences, 0, sizeof(VkFence) * raw_vector_size(&state->swapchain_images_VkImage));

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(state->logical_device, &semaphoreInfo, NULL, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(state->logical_device, &semaphoreInfo, NULL, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(state->logical_device, &fenceInfo, NULL, &frameFences[i]) != VK_SUCCESS) {
            log_fatal("Could not create sempahores\n");
            exit(EXIT_FAILURE);
        }
    }

    size_t current_frame = 0;
    while (!glfwWindowShouldClose(state->window)) {
        glfwPollEvents();
        
        vkWaitForFences(state->logical_device, 1, &frameFences[current_frame], VK_TRUE, UINT64_MAX);

        //
        // Acquire swapchain image. Signal imageAvailableSemaphore when done
        //
        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(
            state->logical_device, 
            state->swapchain, 
            UINT64_MAX, 
            imageAvailableSemaphores[current_frame], 
            VK_NULL_HANDLE, 
            &imageIndex);

        //
        // Check to see if current swapchain is out of date.
        // If so, recreate it.
        //
        if (result == VK_ERROR_OUT_OF_DATE_KHR || glfw_window_resized) {
            glfw_window_resized = false;
            vulkan_swapchain_recreate(state);
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            log_fatal("failed to acquire swapchain image!\n");
            exit(EXIT_FAILURE);
        }

        //
        // We want to prevent the following situation: two frames simultaneously
        // in flight grab the same image. Then, one submits the draw command to
        // the image's queue. Before this draw command is finished, the next one also
        // submits its draw command to the queue. Even though there is the imageAvailableSemaphore
        // that this subsequent draw command must wait for, it is still not good to have
        // those two commands in the queue for the same image at the same time. The command
        // buffer is (probably) not marked for simultaneous use, so this will error.
        //
        if (imageFences[imageIndex] != VK_NULL_HANDLE) {
            vkWaitForFences(state->logical_device, 1, &imageFences[imageIndex], VK_TRUE, UINT64_MAX);
        }
        imageFences[imageIndex] = frameFences[current_frame];

        //
        // Submit draw command buffer. Wait to output to color attachment
        // until imageAvailable semaphore is signaled. Signal renderFinishedSemaphore
        // when done
        //
        VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[current_frame]};
        VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[current_frame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = (VkCommandBuffer *)raw_vector_get_ptr(&state->command_buffers, imageIndex);
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        vkResetFences(state->logical_device, 1, &frameFences[current_frame]);
        if (vkQueueSubmit(state->graphics_queue, 1, &submitInfo, frameFences[current_frame]) != VK_SUCCESS) {
            log_fatal("failed to submit draw command buffer!\n");
            exit(EXIT_FAILURE);
        }

        //
        // Submit presentation command to presentation queue. Wait on
        // renderFinishedSemaphore to be signaled before proceeding
        //
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &state->swapchain;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = NULL; // Optional
        result = vkQueuePresentKHR(state->presentation_queue, &presentInfo);

        //
        // Check to see if the current swapchain is out of date or suboptimal. 
        // If so, recreate it.
        //
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || glfw_window_resized) {
            glfw_window_resized = false;
            vulkan_swapchain_recreate(state);
        } else if (result != VK_SUCCESS) {
            log_fatal("failed to present swapchain image!\n");
            exit(EXIT_FAILURE);
        }

        current_frame = (current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    vkDeviceWaitIdle(state->logical_device);
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(state->logical_device, imageAvailableSemaphores[i], NULL);
        vkDestroySemaphore(state->logical_device, renderFinishedSemaphores[i], NULL);
        vkDestroyFence(state->logical_device, frameFences[i], NULL);
    }
}

void vulkan_swapchain_recreate(struct VulkanState *state) {

    //
    // Wait until we are not using any swapchain resources
    //
    vkDeviceWaitIdle(state->logical_device);

    //
    // Free all resources dependent on the swapchain
    //
    vkFreeCommandBuffers(
        state->logical_device,
        state->command_pool,
        raw_vector_size(&state->command_buffers),
        (VkCommandBuffer *)raw_vector_get_ptr(&state->command_buffers, 0));
    vkDestroyPipeline(state->logical_device, state->pipeline, NULL);
    vkDestroyPipelineLayout(state->logical_device, state->pipeline_layout, NULL);
    vkDestroyRenderPass(state->logical_device, state->renderpass, NULL);
    for (int i = 0; i < raw_vector_size(&state->swapchain_images_VkImage); i++) {
        vkDestroyFramebuffer(
            state->logical_device, 
            *(VkFramebuffer *)raw_vector_get_ptr(&state->framebuffers_VkFramebuffer, i), 
            NULL);
        vkDestroyImageView(
            state->logical_device, 
            *(VkImageView *)raw_vector_get_ptr(&state->swapchain_image_views_VkImageView, i), 
            NULL);
    }
    vkDestroySwapchainKHR(state->logical_device, state->swapchain, NULL);


    //
    // Generate a new swapchain and all resources that depend on it
    //
    int width, height;
    glfwGetFramebufferSize(state->window, &width, &height);
    state->swapchain = create_swapchain(
        state->logical_device, 
        state->physical_device.physical_device,
        state->surface,
        width,
        height,
        optional_index_get_value(&state->physical_device.graphics_family_index),
        optional_index_get_value(&state->physical_device.presentation_family_index),
        &state->swapchain_format,
        &state->swapchain_extent
        );

    uint32_t image_count;
    vkGetSwapchainImagesKHR(state->logical_device, state->swapchain, &image_count, NULL);
    VkImage images[image_count];
    vkGetSwapchainImagesKHR(state->logical_device, state->swapchain, &image_count, images);
    state->swapchain_images_VkImage = raw_vector_create(sizeof(VkImage), image_count);
    raw_vector_extend_back(&state->swapchain_images_VkImage, images, image_count);

    state->swapchain_image_views_VkImageView = create_swapchain_image_views(
        state->logical_device, state->swapchain_images_VkImage, state->swapchain_format);

    state->renderpass = create_render_pass(state->logical_device, state->swapchain_format);

    state->pipeline = create_graphics_pipeline(
        state->logical_device, 
        state->swapchain_extent, 
        state->renderpass, 
        &state->pipeline_layout);

    state->framebuffers_VkFramebuffer = create_framebuffers(
        state->logical_device, 
        state->renderpass, 
        state->swapchain_extent, 
        &state->swapchain_image_views_VkImageView);

    struct RawVector command_buffers = create_command_buffers(
        state->logical_device,
        state->command_pool,
        state->renderpass,
        state->pipeline,
        &state->framebuffers_VkFramebuffer,
        state->swapchain_extent);
}

//
// Initializes all Vulkan state
//
struct VulkanState vulkan_state_create() {

    GLFWwindow *window = init_window();
    glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);

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

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    VkSwapchainKHR swapchain = create_swapchain(
        logical_device, 
        physical_device.physical_device,
        surface,
        width,
        height,
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

    VkRenderPass renderpass = create_render_pass(logical_device, swapchain_format);

    VkPipelineLayout pipeline_layout;
    VkPipeline pipeline = create_graphics_pipeline(logical_device, swapchain_extent, renderpass, &pipeline_layout);

    struct RawVector framebuffers = create_framebuffers(logical_device, renderpass, swapchain_extent, &swapchain_image_views_VkImageView);

    VkCommandPool pool = create_command_pool(logical_device, optional_index_get_value(&physical_device.graphics_family_index));
    struct RawVector command_buffers = create_command_buffers(
        logical_device,
        pool,
        renderpass,
        pipeline,
        &framebuffers,
        swapchain_extent);

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

        .renderpass = renderpass,
        
        .pipeline = pipeline,
        .pipeline_layout = pipeline_layout, 

        .framebuffers_VkFramebuffer = framebuffers,

        .command_pool = pool,
        .command_buffers = command_buffers,
    };
} 

//
// Cleans up
//
void vulkan_state_destroy(struct VulkanState *state) {

    vkDestroyCommandPool(state->logical_device, state->command_pool, NULL);
    for (int i = 0; i < raw_vector_size(&state->framebuffers_VkFramebuffer); i++) {
        vkDestroyFramebuffer(
            state->logical_device, 
            *(VkFramebuffer *)raw_vector_get_ptr(&state->framebuffers_VkFramebuffer, i), 
            NULL);
    }
    vkDestroyPipelineLayout(state->logical_device, state->pipeline_layout, NULL);
    vkDestroyPipeline(state->logical_device, state->pipeline, NULL);
    vkDestroyRenderPass(state->logical_device, state->renderpass, NULL);
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