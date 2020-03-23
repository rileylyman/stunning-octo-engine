//#define GLM_FORCE_RADIANS
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE //TODO : 0-1 depth! 
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "log/log.h"
#include "vulkan-interface/interface-vk.h"


int main() {
    init_log();

    struct VulkanState vulkan_state = vulkan_state_create();

    while (!glfwWindowShouldClose(vulkan_state.window)) {
        glfwPollEvents();
    }

    vulkan_state_destroy(&vulkan_state);
    return EXIT_SUCCESS;
}
