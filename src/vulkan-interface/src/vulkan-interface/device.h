#ifndef VULKAN_DEVICE_H
#define VULKAN_DEVICE_H

#include <vulkan/vulkan.h>
#include <stdbool.h>
#include <stdlib.h>
#include "log/log.h"
#include <language/optional.h>
#include "vulkan-interface/debug.h"

struct InterfacePhysicalDevice {
   VkPhysicalDevice physical_device;
   struct OptionalIndex graphics_family_index; 
};

struct QueueFamilyIndices queue_family_indices_get_indices(); 

struct InterfacePhysicalDevice pick_physical_device(VkInstance instance); 
bool interface_physical_device_is_device_suitable(struct InterfacePhysicalDevice *device);
bool interface_physical_device_is_complete(struct InterfacePhysicalDevice *indices);
VkDevice create_logical_device(struct InterfacePhysicalDevice *pdev);

#endif