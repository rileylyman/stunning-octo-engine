#include "vulkan-interface/device.h"
#include "vulkan-interface/swapchain.h"
#include "language/raw_vector.h"

//
// For now, we just need devices to support the swapchain extension.
//
const char* required_device_extensions[NUM_DEVICE_EXTENSIONS] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

//
// Return whether the given device contain all the required index values within
// its optional indices
//
bool interface_physical_device_is_complete(struct InterfacePhysicalDevice *ipdev) {
    struct OptionalIndex graphics_family_index = ipdev->graphics_family_index;
    struct OptionalIndex presentation_family_index = ipdev->presentation_family_index;
    return optional_index_has_value(&graphics_family_index) && optional_index_has_value(&presentation_family_index);
}

//
// Get the indices of queue families which satisfy certain properties
// within the list of queue families for this physical device. Populates
// the *device with these indices. 
//
void interface_physical_device_fill_indices(struct InterfacePhysicalDevice *device, VkSurfaceKHR surface) {

    struct OptionalIndex graphics_family_index = optional_index_empty();
    struct OptionalIndex presentation_family_index = optional_index_empty();

    uint32_t queue_family_count;
    vkGetPhysicalDeviceQueueFamilyProperties(device->physical_device, &queue_family_count, NULL);
    VkQueueFamilyProperties queue_family_properties[queue_family_count];
    vkGetPhysicalDeviceQueueFamilyProperties(device->physical_device, &queue_family_count, queue_family_properties);

    for (uint32_t i = 0; i < queue_family_count; i++) {
        //
        // Check to see if this queue family supports graphics by ANDing its queueFlags with VK_QUEUE_GRAPHICS_BIT
        //
        if (!optional_index_has_value(&graphics_family_index) && queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            optional_index_set_value(&graphics_family_index, i);
        }
        //
        // Check to see if this queue family can present to our surface
        //
        VkBool32 presentation_supported = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(device->physical_device, i, surface, &presentation_supported);
        if (presentation_supported) {
            optional_index_set_value(&presentation_family_index, i);
        }
    }

    device->graphics_family_index = graphics_family_index;
    device->presentation_family_index = presentation_family_index;
}

//
// Returns true if this device supports all extensions listed in
// the global required_device_extensions variable, false otherwise
//
bool device_supports_required_extensions(struct InterfacePhysicalDevice *ipdev) {
    uint32_t device_ext_count;
    vkEnumerateDeviceExtensionProperties(ipdev->physical_device, NULL, &device_ext_count, NULL);

    VkExtensionProperties props[device_ext_count];
    vkEnumerateDeviceExtensionProperties(ipdev->physical_device, NULL, &device_ext_count, props);

    for (int i = 0; i < NUM_DEVICE_EXTENSIONS; i++) {
        bool found = false;
        for (int j = 0; j < device_ext_count; j++) {
            if (!strcmp(props[j].extensionName, required_device_extensions[i])) {
                found = true;
                break;
            }
        }
        if (!found) return false;
    }
    return true;
}

//
// Determines if this physical device is suitable by filling its indices
// and checking if it is then complete.
//
bool interface_physical_device_is_device_suitable(struct InterfacePhysicalDevice *ipdev, VkSurfaceKHR surface) {
    log_trace("Checking if device is suitable...\n");

    //
    // We first check that this device has queue families for graphics
    // and presenting to our specified surface.
    //
    interface_physical_device_fill_indices(ipdev, surface);

    //
    // We then need to make sure that our device has the swapchain extension
    // enabled.
    //
    bool device_has_exts = device_supports_required_extensions(ipdev);

    //
    // Now we need to check to see if the swapchain from this device to our surface
    // supports features that we required in terms of supported swapchain formats
    // and presentation modes. 
    //
    bool swapchain_adequate = false;
    if (device_has_exts) swapchain_adequate = is_swapchain_adequate(ipdev->physical_device, surface);

    bool suitable = interface_physical_device_is_complete(ipdev) && device_has_exts && swapchain_adequate;

    log_trace("Device is%ssuitable!\n", suitable ? "" : " not ");
    return suitable;
}

//
// Lists the physical devices available to this instance. Selects the
// first physical device with the required queue families and features.
//
struct InterfacePhysicalDevice pick_physical_device(VkInstance instance, VkSurfaceKHR surface) {
    VkResult int_result;
    struct InterfacePhysicalDevice physical_device = {};
    bool found = false;

    //
    // Get an array of all devices which are compatible with
    // this instance
    //
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, NULL);
    if (device_count == 0) {
        log_fatal("No devices could be found for instance.\n");
        exit(EXIT_FAILURE);
    }
    VkPhysicalDevice devices[device_count];
    vkEnumeratePhysicalDevices(instance, &device_count, devices);

    for (int i = 0; i < device_count; i++) {
        struct InterfacePhysicalDevice dev = {
            .physical_device = devices[i],
            .graphics_family_index = optional_index_empty(),
            .presentation_family_index = optional_index_empty(),
        };
        if (interface_physical_device_is_device_suitable(&dev, surface)) {
            //
            // For now, we just choose the first physical device which matches
            //
            physical_device = dev;
            found = true;
            break;
        }
    }
    if (!found) {
        log_fatal("No device matches requirements. Aborting...\n");
        exit(EXIT_FAILURE);
    }

    return physical_device;
}

//
// Create a logical device from a physical device. Creates queue
// create infos for all required queues for the required queue families.
//
VkDevice create_logical_device(struct InterfacePhysicalDevice *pdev) {

    float queue_priorities[1] = { 1.0f };

    struct RawVector queue_create_info_list = raw_vector_create(sizeof(VkDeviceQueueCreateInfo), 2);

    raw_vector_push_back(&queue_create_info_list, &(VkDeviceQueueCreateInfo) {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pQueuePriorities = queue_priorities,
        .queueFamilyIndex = optional_index_get_value(&pdev->graphics_family_index),
        .queueCount = 1,
    });
    if (optional_index_get_value(&pdev->graphics_family_index) != optional_index_get_value(&pdev->presentation_family_index)) {
        raw_vector_push_back(&queue_create_info_list, &(VkDeviceQueueCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pQueuePriorities = queue_priorities,
            .queueFamilyIndex = optional_index_get_value(&pdev->presentation_family_index),
            .queueCount = 1,
        });
    }

    VkPhysicalDeviceFeatures features = {};

    VkDeviceCreateInfo device_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pEnabledFeatures = &features,
#ifdef NDEBUG
        .enabledLayerCount = 0;
#else
        .ppEnabledLayerNames = debug_requested_validation_layers,
        .enabledLayerCount = NUM_VALIDATION_LAYERS,
#endif
        .queueCreateInfoCount = raw_vector_size(&queue_create_info_list),
        .pQueueCreateInfos = (VkDeviceQueueCreateInfo *)raw_vector_get_ptr(&queue_create_info_list, 0),

        .enabledExtensionCount = NUM_DEVICE_EXTENSIONS,
        .ppEnabledExtensionNames = required_device_extensions,
    };

    VkDevice logical_device;
    if (vkCreateDevice(pdev->physical_device, &device_create_info, NULL, &logical_device) != VK_SUCCESS) {
        log_fatal("Failed to create logical device.\n");
        exit(EXIT_FAILURE);
    }

    log_trace("Created logical device with %u queues\n", raw_vector_size(&queue_create_info_list));

    raw_vector_destroy(&queue_create_info_list);
    return logical_device;
}
