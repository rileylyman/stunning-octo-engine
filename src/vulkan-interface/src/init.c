#include "vulkan-interface/init.h"

//
// Creates the GLFW window
//
GLFWwindow *init_window() {
    //
    // Initialize GLFW with no API (no OpenGL)
    //
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    return glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, NULL, NULL);
}

//
// Initializes Vulkan and creates an instance object. Queries the extension.h
// module for the required extensions. Queries the debug.h module for the required
// validation layers. Then it creates the Vulkan instance and returns it.
//
VkInstance init_vulkan() {

    VkResult intResult;

    //
    // Checking for extensions
    //
    struct RawVector requiredExtensions = get_required_extension_names_FREE();

    uint32_t totalExtensionCount;
    vkEnumerateInstanceExtensionProperties(NULL, &totalExtensionCount, NULL);
    VkExtensionProperties extensions[totalExtensionCount];
    vkEnumerateInstanceExtensionProperties(NULL, &totalExtensionCount, extensions);

    log_trace("Available extensions:");
    for (int i = 0; i < totalExtensionCount; i++) {
        printf("\t%s\n", extensions[i].extensionName);
    }
    log_trace("\nGLFW Requested extensions:");
    for (int i = 0; i < raw_vector_size(&requiredExtensions); i++) {
        printf("\t%s\n", *(char **)raw_vector_get_ptr(&requiredExtensions, i));
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

        log_trace("Requested layer: %s\n", debug_requested_validation_layers[i]);

        bool layerFound = false;
        for (int j = 0; j < layerCount; j++) {
            log_trace("Found layer: %s\n", layers[j].layerName);
            if (!strcmp(debug_requested_validation_layers[i], layers[j].layerName)) {
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
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = get_debug_create_info();

    VkInstanceCreateInfo createInfo = {
        .sType                      = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo           = &app_info,
        .enabledExtensionCount      = raw_vector_size(&requiredExtensions),
        .ppEnabledExtensionNames    = (const char**)requiredExtensions.data,
#ifdef NDEBUG
        .enabledLayerCount          = 0,
#else
        .enabledLayerCount          = NUM_VALIDATION_LAYERS,
        .ppEnabledLayerNames        = debug_requested_validation_layers,
        .pNext                      = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo,
#endif
    };

    VkInstance instance;
    VkResult result = vkCreateInstance(&createInfo, NULL, &instance);

    if (result != VK_SUCCESS) {
        log_error("Error creating VkInstance: %u\n", result);
        exit(EXIT_FAILURE);
    }

    raw_vector_destroy(&requiredExtensions);
    return instance;
}

VkSurfaceKHR create_surface(VkInstance instance, GLFWwindow *window) {
    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(instance, window, NULL, &surface) != VK_SUCCESS) {
        log_fatal("Failed to create VkSurface!\n");
        exit(EXIT_FAILURE);
    }
    return surface;
}
