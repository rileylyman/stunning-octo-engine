#include <stdbool.h>
#include <stdlib.h>
#include "log/log.h"
#include "vulkan-interface/device.h"
#include "language/optional.h"

//
// Return whether the given Indices contain all the required values within
// them. 
//
bool queue_family_indices_is_complete(struct QueueFamilyIndices *indices) {
    return optional_index_has_value(&indices->graphics_family_index);
}

//
// Get the indices of queue families which satisfy certain properties
// within the list of queue families for this physical device. 
//
struct QueueFamilyIndices queue_family_indices_get_indices(VkPhysicalDevice device) {

    struct OptionalIndex graphics_family_index = optional_index_empty();

    uint32_t queue_family_count;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, NULL);

    VkQueueFamilyProperties queue_family_properties[queue_family_count];
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_family_properties);

    for (uint32_t i = 0; i < queue_family_count; i++) {
        if (queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            optional_index_set_value(&graphics_family_index, i);
            break;
        }
    }

    return (struct QueueFamilyIndices) {
        .graphics_family_index = graphics_family_index
    };
}

bool is_device_suitable(VkPhysicalDevice device) {
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceProperties(device, &properties);
    vkGetPhysicalDeviceFeatures(device, &features);

    struct QueueFamilyIndices indices = queue_family_indices_get_indices(device);
    
    return queue_family_indices_is_complete(&indices);
}

VkPhysicalDevice pick_physical_device(VkInstance instance) {
    VkResult intResult;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    //
    // Get an array of all devices which are compatible with
    // this instance
    //
    uint32_t deviceCount = 0;
    intResult = vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
    if (intResult != VK_SUCCESS) {
        log_fatal("Could not enumerate physical devices.\n");
        exit(EXIT_FAILURE);
    }
    else if (deviceCount == 0) {
        log_fatal("No devices could be found for instance.\n");
        exit(EXIT_FAILURE);
    }
    VkPhysicalDevice devices[deviceCount];
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices);

    for (int i = 0; i < deviceCount; i++) {
        VkPhysicalDevice dev = devices[i];
        if (is_device_suitable(dev)) {
            //
            // For now, we just choose the first physical device which matches
            //
            physicalDevice = dev;
            break;
        }
    }
    if (physicalDevice == VK_NULL_HANDLE) {
        log_fatal("No device matches requirements. Aborting...\n");
        exit(EXIT_FAILURE);
    }

    return physicalDevice;
}