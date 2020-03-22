//
// This header deals with creating the Debug Messenger
// for the program as well as defining the debug callback
// function which is called by the validation layers.
// We also define the log level in this file. 
//
#ifndef VULKAN_DEBUG_H
#define VULKAN_DEBUG_H

#include <vulkan/vulkan.h>
#include "vulkan-interface.h"
#include "log/log.h"
#include <stdlib.h>

#define LOG_LEVEL LOG_TRACE

#define NUM_VALIDATION_LAYERS 1
const char *validationLayers[NUM_VALIDATION_LAYERS] = {
    "VK_LAYER_KHRONOS_validation"
};


VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData); 


VkDebugUtilsMessengerEXT initVulkanDebugMessenger(VkInstance instance); 

VkDebugUtilsMessengerCreateInfoEXT getDebugCreateInfo(); 

void initLog();

#endif
