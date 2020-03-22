#include "vulkan-interface/debug.h"

const char *debug_requested_validation_layers[NUM_VALIDATION_LAYERS] = {
    "VK_LAYER_KHRONOS_validation"
};

//
// This is the debug callback function called by the validation
// layers
//
VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) { 

    //
    // Simply log the message recieved by the validation layer
    //
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        log_error("%s\n", pCallbackData->pMessage);
    } 
    else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        log_warn("%s\n", pCallbackData->pMessage);
    }
    else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        log_info("%s\n", pCallbackData->pMessage);
    }
    else {
        log_debug("%s\n", pCallbackData->pMessage);
    }

    return VK_FALSE; // Should we abort the Vulkan call?
}

//
// This function is responsible for creating the debug messenger
// used by this program
//
VkDebugUtilsMessengerEXT init_vulkan_debug_messenger(VkInstance instance) {

    VkDebugUtilsMessengerCreateInfoEXT createInfo = get_debug_create_info();
    VkDebugUtilsMessengerEXT debugMessenger;
    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, NULL, &debugMessenger) != VK_SUCCESS) {
        log_fatal("Could not create debug messenger!\n");
        exit(EXIT_FAILURE);
    }
    return debugMessenger;
}

//
// Returns the VkDebugUtilsMessengerCreateInfoEXT object containing
// the desired debug parameters for this module.
//
VkDebugUtilsMessengerCreateInfoEXT get_debug_create_info() {
    return (VkDebugUtilsMessengerCreateInfoEXT){
        .sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT 
                         | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT 
                         | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT 
                         | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT 
                         | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = &debug_callback,
        .pUserData       = NULL,
    };
}

//
// This function sets the logging level to what was def'd in debug.h
//
inline void init_log() {
    log_set_level(LOG_LEVEL);
}
