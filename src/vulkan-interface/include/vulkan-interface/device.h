#ifndef VULKAN_DEVICE_H
#define VULKAN_DEVICE_H

#include <vulkan/vulkan.h>
#include <stdbool.h>
#include <stdlib.h>
#include "log.h"
#include <language/optional.h>
#include "vulkan-interface/debug.h"

#define NUM_DEVICE_EXTENSIONS 1
const char* required_device_extensions[NUM_DEVICE_EXTENSIONS];

struct InterfacePhysicalDevice {
   VkPhysicalDevice physical_device;
   struct OptionalIndex graphics_family_index; 
   struct OptionalIndex presentation_family_index;
};

void interface_physical_device_fill_indices(struct InterfacePhysicalDevice *device, VkSurfaceKHR surface); 

struct InterfacePhysicalDevice pick_physical_device(VkInstance instance, VkSurfaceKHR surface); 
bool interface_physical_device_is_device_suitable(struct InterfacePhysicalDevice *device, VkSurfaceKHR surface);
bool interface_physical_device_is_complete(struct InterfacePhysicalDevice *indices);
bool device_supports_required_extensions(struct InterfacePhysicalDevice *ipdev); 
VkDevice create_logical_device(struct InterfacePhysicalDevice *pdev);

#endif