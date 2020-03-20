#include "vulkan-interface/extension.h"

//
// Retrieves the vkCreateDebugUtilsMessengerEXT function contained
// within the debug utils extension
//
VkResult CreateDebugUtilsMessengerEXT(
        VkInstance instance, 
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
        const VkAllocationCallbacks* pAllocator, 
        VkDebugUtilsMessengerEXT* pDebugMessenger) {

    PFN_vkCreateDebugUtilsMessengerEXT func = 
        (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
                instance, 
                "vkCreateDebugUtilsMessengerEXT"
                );

    if (func != NULL) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        log_error("Could not find vkCreateDebugUtilsMessengerEXT\n");
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

//
// Obtains the vkDestroyDebugUtilsMessengerEXT function contained
// within the debug utils extension
//
void DestroyDebugUtilsMessengerEXT(
        VkInstance instance, 
        VkDebugUtilsMessengerEXT debugMessenger, 
        const VkAllocationCallbacks* pAllocator) {

    PFN_vkDestroyDebugUtilsMessengerEXT func = 
        (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
                instance, 
                "vkDestroyDebugUtilsMessengerEXT"
                );

    if (func != NULL) {
        func(instance, debugMessenger, pAllocator);
    } else {
        log_error("Could not find vkDestroyDebugUtilsMessengerEXT\n");
    }
}

//
// Get extensions required by the program. This includes the GLFW
// required extensions along with the debug utils extension if
// validation layers are to be enabled.
//
const char** getRequiredExtensionNames_FREE(uint32_t* count) {
    //
    // TODO : This is awful
    //
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(count);
#ifndef NDEBUG
    (*count)++;
#endif
    const char** extensions = (const char**)malloc(sizeof(const char *) * *count);
#ifndef NDEBUG
    (*count)--;
#endif
    memcpy(extensions, glfwExtensions, *count * sizeof(const char *));
#ifndef NDEBUG
    (*count)++;
    extensions[*count - 1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
#endif
    return extensions;

}
