//
// This file takes care of initializing the GLFW window
// and the Vulkan instance
//
#ifndef VULKAN_INIT_H
#define VULKAN_INIT_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include "log/log.h"
#include "vulkan-interface/debug.h"
#include "vulkan-interface/extension.h"

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600
#define WINDOW_NAME   "Vulkan"

static VkApplicationInfo app_info = {
    .sType                = VK_STRUCTURE_TYPE_APPLICATION_INFO, 
    .pApplicationName     = "Tiles",
    .applicationVersion   = VK_MAKE_VERSION(1, 0, 0),
    .pEngineName          = "Tiling Engine",
    .engineVersion        = VK_MAKE_VERSION(1, 0, 0),
    .apiVersion           = VK_API_VERSION_1_0,
};

GLFWwindow *init_window();
VkInstance init_vulkan(); 
VkSurfaceKHR create_surface(VkInstance instance, GLFWwindow *window);

#endif
