//
// This header deals with creating the Debug Messenger
// for the program as well as defining the debug callback
// function which is called by the validation layers.
// We also define the log level in this file. 
//
#ifndef VULKAN_DEBUG_H
#define VULKAN_DEBUG_H

#include <vulkan/vulkan.h>
#include "log/log.h"
#include <stdlib.h>
#include <stdbool.h>
#include "vulkan-interface/extension.h"

#define LOG_LEVEL LOG_TRACE

#define NUM_VALIDATION_LAYERS 1
const char *debug_requested_validation_layers[NUM_VALIDATION_LAYERS];

VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData); 


VkDebugUtilsMessengerEXT init_vulkan_debug_messenger(VkInstance instance); 

VkDebugUtilsMessengerCreateInfoEXT get_debug_create_info(); 

void init_log();

#endif
