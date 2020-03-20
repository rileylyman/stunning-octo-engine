#include "vulkan-interface/init.h"

//
// Creates the GLFW window
//
GLFWwindow *initWindow() {
    //
    // Initialize GLFW with no API (no OpenGL)
    //
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    return glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, NULL, NULL);
}

//
// Initializes Vulkan and creates an instance object. Queries the extension.h
// module for the required extensions. Queries the debug.h module for the required
// validation layers. Then it creates the Vulkan instance and returns it.
//
VkInstance initVulkan() {

    VkResult intResult;

    //
    // Checking for extensions
    //
    uint32_t extensionCount;
    const char** requiredExtensions = getRequiredExtensionNames_FREE(&extensionCount);

    uint32_t totalExtensionCount;
    vkEnumerateInstanceExtensionProperties(NULL, &totalExtensionCount, NULL);
    VkExtensionProperties extensions[totalExtensionCount];
    vkEnumerateInstanceExtensionProperties(NULL, &totalExtensionCount, extensions);

    log_trace("Available extensions:");
    for (int i = 0; i < totalExtensionCount; i++) {
        printf("\t%s\n", extensions[i].extensionName);
    }
    log_trace("\nGLFW Requested extensions:");
    for (int i = 0; i < extensionCount; i++) {
        printf("\t%s\n", requiredExtensions[i]);
    }

    //
    // Checking for validation layers
    //
    uint32_t layerCount;
    intResult = vkEnumerateInstanceLayerProperties(&layerCount, NULL);
    if (intResult != VK_SUCCESS) {
        log_fatal("Error retrieving instance layer properties!\n");
    }
    VkLayerProperties layers[layerCount];
    vkEnumerateInstanceLayerProperties(&layerCount, layers);

    log_trace("Found %u possible validation layers\n", layerCount);

    bool allLayersFound = true;
    for (int i = 0; i < NUM_VALIDATION_LAYERS; i++) {

        log_trace("Requested layer: %s\n", validationLayers[i]);

        bool layerFound = false;
        for (int j = 0; j < layerCount; j++) {
            log_trace("Found layer: %s\n", layers[j].layerName);
            if (!strcmp(validationLayers[i], layers[j].layerName)) {
                layerFound = true;
                log_trace(" \t... layer matches!\n");
                break;
            }
        }
        if (!layerFound) { allLayersFound = false; break; }
    }
    if (!allLayersFound) {
        log_fatal("Not all requested validation layers are available!\n");
        exit(EXIT_FAILURE);
    }
    log_trace("All requested validation layers found.\n");

    //
    // Create the Vulkan instance. Use the DebugCreateInfo struct as the
    // pNext pointer in the instance create info in order to enable debugging
    // of vkCreateInstance and vkDestroyInstance calls.
    //
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = getDebugCreateInfo();

    VkInstanceCreateInfo createInfo = {
        .sType                      = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo           = &appInfo,
        .enabledExtensionCount      = extensionCount,
        .ppEnabledExtensionNames    = requiredExtensions,
#ifdef NDEBUG
        .enabledLayerCount          = 0,
#else
        .enabledLayerCount          = NUM_VALIDATION_LAYERS,
        .ppEnabledLayerNames        = validationLayers,
        .pNext                      = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo,
#endif
    };

    VkInstance instance;
    VkResult result = vkCreateInstance(&createInfo, NULL, &instance);

    if (result != VK_SUCCESS) {
        log_error("Error creating VkInstance: %u\n", result);
        exit(EXIT_FAILURE);
    }

    free(requiredExtensions);
    return instance;
}

