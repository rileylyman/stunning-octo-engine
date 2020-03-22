//#define GLM_FORCE_RADIANS
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE //TODO : 0-1 depth! 
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "log/log.h"
#include "vulkan-interface/interface-vk.h"


int main() {
    init_log();

    GLFWwindow *window = init_window();
    VkInstance instance = init_vulkan();
#ifndef NDEBUG
    VkDebugUtilsMessengerEXT debugMessenger = init_vulkan_debug_messenger(instance);
#endif

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

#ifndef NDEBUG
    DestroyDebugUtilsMessengerEXT(instance, debugMessenger, NULL);
#endif
    vkDestroyInstance(instance, NULL);
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
