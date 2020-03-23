#include "vulkan-interface/extension.h"
#include "language/raw_vector.h"

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
const char** get_required_extension_names_FREE(uint32_t* count) {

    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(count);

    struct RawVector extension_names = raw_vector_create(sizeof(char *), *count);
    raw_vector_extend_back(&extension_names, glfwExtensions, *count);

#ifndef NDEBUG
    const char *debug_ext = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    raw_vector_push_back(&extension_names, debug_ext);
    *count += 1;
#endif

    return (const char **) extension_names.data;
}
