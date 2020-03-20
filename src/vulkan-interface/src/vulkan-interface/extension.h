//
// Manages wrangling of extension functions and construction
// of a list of required extensions
//
#ifndef VULKAN_EXT_H
#define VULKAN_EXT_H

#include <stdlib.h>
#include <string.h>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include "log/log.h"

VkResult CreateDebugUtilsMessengerEXT(
        VkInstance instance, 
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
        const VkAllocationCallbacks* pAllocator, 
        VkDebugUtilsMessengerEXT* pDebugMessenger); 

void DestroyDebugUtilsMessengerEXT(
        VkInstance instance, 
        VkDebugUtilsMessengerEXT debugMessenger, 
        const VkAllocationCallbacks* pAllocator); 

const char** getRequiredExtensionNames_FREE();

#endif
