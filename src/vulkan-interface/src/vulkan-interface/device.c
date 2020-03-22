#include <stdbool.h>
#include <stdlib.h>
#include "log/log.h"
#include "vulkan-interface/device.h"
#include "language/optional.h"

bool isDeviceSuitable(VkInstance instace, VkPhysicalDevice device) {
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceProperties(device, &properties);
    vkGetPhysicalDeviceFeatures(device, &features);

    //
    // TODO: Do something here
    //

    return true;
}

VkPhysicalDevice pickPhysicalDevice(VkInstance instance) {
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
        if (isDeviceSuitable(instance, dev)) {
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