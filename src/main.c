//#define GLM_FORCE_RADIANS
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE //TODO : 0-1 depth! 

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "log/log.h"
#include "vulkan-interface/extension.h"
#include "vulkan-interface/init.h"
#include "vulkan-interface/debug.h"


int main() {
    initLog();

    GLFWwindow *window = initWindow();
    VkInstance instance = initVulkan();
#ifndef NDEBUG
    VkDebugUtilsMessengerEXT debugMessenger = initVulkanDebugMessenger(instance);
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
